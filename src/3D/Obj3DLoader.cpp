#include "3D/Obj3DLoader.h"

#include <iostream>
#include <sstream>
#include <string>

#include "3D/Box3D.h"
#include "3D/Face3D.h"

//#define DEBUG_TRACE_OBJLOADER
//#define DEBUG_TRACE_OBJLOADER_VERBOSE

Obj3DLoader::Obj3DLoader()
{
  //ctor
#ifdef DEBUG_TRACE_OBJLOADER
  std::cout << "Obj3DLoader" << this << " created" << std::endl;
#endif
}

Obj3DLoader::~Obj3DLoader()
{
  //dtor
#ifdef DEBUG_TRACE_OBJLOADER
  std::cout << "Obj3DLoader" << this << " destroyed" << std::endl;
#endif
}

void Obj3DLoader::reset()
{
  this->indexed_v.clear();
  this->indexed_vt.clear();
  this->indexed_vn.clear();
  this->linear_points.clear();
  this->subobject_mesh.clear();
  this->subobject_material.clear();
  this->subobject_start.clear();
  this->subobject_length.clear();
  this->unique_v.clear();
  this->subobject_no = -1;
  this->box = nullptr;
}


void Obj3DLoader::begin_mesh(std::string filename, std::string name)
{
#ifdef DEBUG_TRACE_OBJLOADER
  std::cout << "Obj3DLoader" << this << "::begin_mesh() loading " << name << " from " << filename << std::endl;
#endif
  // Create mesh object
  SubObject3D* mesh = new SubObject3D();
  mesh->setFilename(filename);
  mesh->setName(name);

  // Create and assign bounding box
  this->box = new Box3D();
  this->box->setName(name);
  mesh->setBounds(this->box);

  // Add mesh to container object
  this->object->addPart(mesh);
  this->object->setName(this->filename);
  this->subobject_mesh.push_back(mesh);

  // Set this mesh as "current"
  this->current_mesh = mesh;
}



Obj3D* Obj3DLoader::load(std::string filename)
{
#ifdef DEBUG_TRACE_OBJLOADER
  std::cout << "Obj3DLoader" << this << "::load() filename=" << filename << std::endl;
#endif
  reset();
  int errors = 0;
  this->scanner = new FileScanner();
  if (scanner->load(filename) == false) {
    delete this->scanner;
    std::cerr << "Obj3DLoader::load() error reading " << filename << std::endl;
    return nullptr;
  }
  this->filename = filename;
  this->object = new Obj3D();


#ifdef DEBUG_TRACE_OBJLOADER
  std::cout << "Obj3DLoader::load() begin parse loop" << std::endl;
#endif
  while (!scanner->is_eof()) {
    scanner->skip_whitespace(true);
    if (scanner->is_alpha()) { get_keyword(); continue; }

    std::cerr << "Obj3DLoader" << this << "::load() unexpected character code " << (int)this->scanner->what() << " at " << this->scanner->where() << std::endl;
    errors++;
    scanner->advance();
  }
  if (errors > 0) {
    std::cerr << "Obj3DLoader::load() errors parsing " << filename << ": " << errors << std::endl;
  }

  delete this->scanner;
#ifdef DEBUG_TRACE_OBJLOADER
  std::cout << "Obj3DLoader::load() parsing complete, produce object" << std::endl;
#endif

  object->setName(filename);
  float* v_array = make_v_array();
  float* vt_array = make_vt_array();
  float* vn_array = make_vn_array();
  // Now copy the array data for each sub object using the offsets and lengths
  for (unsigned int i=0; i<subobject_mesh.size(); i++) {
    SubObject3D* mesh = this->subobject_mesh[i];
    mesh->setMaterial(this->subobject_material[i]);
    int offset = this->subobject_start[i];
    int length = this->subobject_length[i];
    mesh->set_v(v_array+offset*3, length);
    mesh->set_vt(vt_array+offset*2, length);
    mesh->set_vn(vn_array+offset*3, length);
    mesh->findAdjacentFaces();
  }
  free(v_array);
  free(vt_array);
  free(vn_array);
  return object;
}

float* Obj3DLoader::make_v_array()
{
  float* arr = (float*) malloc(3 * sizeof(float) * linear_points.size());
  int i = 0;
  for (auto & p : linear_points) {
    arr[i++] = p.v.x;
    arr[i++] = p.v.y;
    arr[i++] = p.v.z;
  }
  return arr;
}

float* Obj3DLoader::make_vt_array()
{
  float* arr = (float*) malloc(2 * sizeof(float) * linear_points.size());
  int i = 0;
  for (auto & p : linear_points) {
    arr[i++] = p.vt.x;
    arr[i++] = p.vt.y;
  }
  return arr;
}

float* Obj3DLoader::make_vn_array()
{
  float* arr = (float*) malloc(3 * sizeof(float) * linear_points.size());
  int i = 0;
  for (auto & p : linear_points) {
    arr[i++] = p.vn.x;
    arr[i++] = p.vn.y;
    arr[i++] = p.vn.z;
  }
  return arr;
}



void Obj3DLoader::get_keyword()
{
  std::string keyword = scanner->get_keyword();
#ifdef DEBUG_TRACE_OBJLOADER_VERBOSE
  std::cout << "Obj3DLoader" << this << "::get_keyword() got keyword=" << keyword << std::endl;
#endif
  if (keyword == "mtllib") { get_mtllib(); return; } // material definition *IGNORED*
  if (keyword == "usemtl") { get_usemtl(); return; } // material reference *IGNORED*
  if (keyword == "f") { get_f(); return; } // face (triangle or quad)
  if (keyword == "l") { get_l(); return; } // line element *IGNORED*
  if (keyword == "o") { get_o(); return; } // object (or subobject)
  if (keyword == "s") { get_s(); return; } // smooth shading group (for vertex normal calculation) *IGNORED*
  if (keyword == "v") { get_v(); return; } // vertex coordinate xyz
  if (keyword == "vt") { get_vt(); return; } // vertex texture coordinate uv
  if (keyword == "vn") { get_vn(); return; } // vertex normal coordinate xyz
  if (keyword == "vp") { get_vp(); return; } // vertex parameter coordinate xyz *IGNORED*

  std::cout << "Obj3DLoader::get_keyword() unhandled keyword=" << keyword << std::endl;
}

void Obj3DLoader::get_mtllib()
{
  // Get name of material library file
  std::string fname = scanner->get_filename();
  std::string path = IOFile::get_path(this->filename);
  this->mtllib_fname = path + fname;
#ifdef DEBUG_TRACE_OBJLOADER
  std::cout << "Obj3DLoader" << this << "::get_mtllib() filename=" << this->mtllib_fname << std::endl;
#endif
  //// Define material - Ignore for now
  //while (!scanner->is_eof() && !scanner->is_eol()) { scanner->advance(); }
  scanner->consume_eol();
}

void Obj3DLoader::get_usemtl()
{
  // Get material name
  std::string mtllib_name = scanner->get_string();
#ifdef DEBUG_TRACE_OBJLOADER
  std::cout << "Obj3DLoader" << this << "::get_usemtl() material name=" << mtllib_name << std::endl;
#endif

  // Load material
  MtlLoader* mtlloader = new MtlLoader();
  Material* material = mtlloader->load(this->mtllib_fname, mtllib_name);
  delete mtlloader;

  // Store material so we can link it to the mesh object later
  subobject_material.push_back(material);

  scanner->consume_eol();
}

void Obj3DLoader::get_f()
{
#ifdef DEBUG_TRACE_OBJLOADER_VERBOSE
  std::cout << "Obj3DLoader" << this << "::get_f() " << scanner->where() << std::endl;
#endif
  // f 2563/2575/2561 2565/2576/2561 2566/2577/2561 2564/2578/2561
  std::vector<Vertex3D> face;
  while (scanner->is_numeric()) {
    // Get mandatory v index
    int v_no = scanner->get_integer();
    int vt_no = -1;
    int vn_no = -1;
    // Look for optional vt index
    if (scanner->is_delimiter()) {
      scanner->advance();
      if (scanner->is_numeric()) {
        vt_no = scanner->get_integer();
      }
      // Look for optional vn index
      if (scanner->is_delimiter()) {
        scanner->advance();
        if (scanner->is_numeric()) {
          vn_no = scanner->get_integer();
        }
      }
    }
    //std::cout << "Obj3DLoader::get_f() point v=" << v_no << " vt=" << vt_no << " vn=" << vn_no << std::endl;
    Vertex3D p = Vertex3D(indexed_v[v_no - 1]);
    //p.v = indexed_v[v_no-1];
    int first_vertex = this->unique_v[this->subobject_no];
    p.vt = (vt_no==-1 ? Vector2(0.0f, 0.0f) : indexed_vt[vt_no-1]);
    p.vn = (vt_no==-1 ? Vector3(0.0f, 0.0f, 0.0f) : indexed_vn[vn_no-1]);
    p.index = v_no - 1 - first_vertex; // Need this when looking for neighbor polygons
    face.push_back(p);
    scanner->skip_whitespace();
  }
  //std::cout << "Obj3DLoader::get_f() face points:" << face.size() << std::endl;
  switch (face.size()) {
    case 3: add_triangle(face); break;
    case 4: add_quad(face); break;
    default:
      std::cout << "Obj3DLoader::get_f() invalid face with " << face.size() << " points" << std::endl;
  }
  scanner->consume_eol();
}

void Obj3DLoader::get_l()
{
#ifdef DEBUG_TRACE_OBJLOADER
  std::cout << "Obj3DLoader" << this << "::get_l() " << scanner->where() << std::endl;
#endif
  // Line element - Ignore for now
  while (!scanner->is_eof() && !scanner->is_eol()) { scanner->advance(); }
  scanner->consume_eol();
}

void Obj3DLoader::get_o()
{
#ifdef DEBUG_TRACE_OBJLOADER
  std::cout << "Obj3DLoader" << this << "::get_o() " << scanner->where() << std::endl;
#endif
  this->subobject_no++;
  // Get subobject name
  std::string name = scanner->get_keyword();
  while (!scanner->is_eof() && !scanner->is_eol()) { scanner->advance(); }
  scanner->consume_eol();
  // Prefix name with subobject number
  name = std::to_string(this->subobject_no).append(":").append(name);

  begin_mesh(this->filename, name);

  // Mark the beginning of sub-object ("part")
  this->subobject_start.push_back((int)this->linear_points.size());
  this->subobject_length.push_back(0);
  this->unique_v.push_back((int)this->indexed_v.size());
}

void Obj3DLoader::get_s()
{
#ifdef DEBUG_TRACE_OBJLOADER
  std::cout << "Obj3DLoader" << this << "::get_s() " << scanner->where() << std::endl;
#endif
  // Ignore for now
  while (!scanner->is_eof() && !scanner->is_eol()) { scanner->advance(); }
  scanner->consume_eol();
}

void Obj3DLoader::get_v()
{
#ifdef DEBUG_TRACE_OBJLOADER_VERBOSE
  std::cout << "Obj3DLoader" << this << "::get_v() " << scanner->where() << std::endl;
#endif
  float x = scanner->get_float();
  float y = scanner->get_float();
  float z = scanner->get_float();
  Vector3 v = Vector3(x, y, z);
  this->current_mesh->addVertex(Vertex3D(v));
  indexed_v.push_back(v);
  this->box->extend(v);
  scanner->consume_eol();
}

void Obj3DLoader::get_vt()
{
#ifdef DEBUG_TRACE_OBJLOADER_VERBOSE
  std::cout << "Obj3DLoader" << this << "::get_vt() " << scanner->where() << std::endl;
#endif
  float u = scanner->get_float();
  float v = scanner->get_float();
  indexed_vt.push_back(Vector2(u, v));
  scanner->consume_eol();
}

void Obj3DLoader::get_vn()
{
#ifdef DEBUG_TRACE_OBJLOADER_VERBOSE
  std::cout << "Obj3DLoader" << this << "::get_vn() " << scanner->where() << std::endl;
#endif
  float x = scanner->get_float();
  float y = scanner->get_float();
  float z = scanner->get_float();
  indexed_vn.push_back(Vector3(x, y, z));
  scanner->consume_eol();
}

void Obj3DLoader::get_vp()
{
#ifdef DEBUG_TRACE_OBJLOADER_VERBOSE
  std::cout << "Obj3DLoader" << this << "::get_vp() " << scanner->where() << std::endl;
#endif
  // Vertex parameter - Ignore for now
  while (!scanner->is_eof() && !scanner->is_eol()) { scanner->advance(); }
  scanner->consume_eol();
}


std::vector<Vertex3D> Obj3DLoader::compute_vn(std::vector<Vertex3D> points)
{
  Vector3 normal = Face3D(&points[0], &points[1], &points[2]).normal;
  points[0].vn = normal;
  points[1].vn = normal;
  points[2].vn = normal;
  return points;
}

void Obj3DLoader::add_triangle(std::vector<Vertex3D> points)
{
#ifdef DEBUG_TRACE_OBJLOADER_VERBOSE
  std::cout << "Obj3DLoader::add_triangle() subobject_no=" << this->subobject_no << std::endl;
#endif
  if (points[0].vn.x == 0.0 && points[0].vn.x == 0.0 && points[0].vn.x == 0.0) {
    points = compute_vn(points);
  }

  // For shadow volumes, take pointers to each vertex of the face
  // because later we will need to look up the connected faces
  Vertex3D* v1p = &this->current_mesh->vertices[points[0].index];
  Vertex3D* v2p = &this->current_mesh->vertices[points[1].index];
  Vertex3D* v3p = &this->current_mesh->vertices[points[2].index];
  Face3D face = Face3D(v1p, v2p, v3p);
  uint32_t face_id = this->current_mesh->addFace(face);

  for (auto& p : points) {
    linear_points.push_back(p); // For rendering, take each vertex
    this->current_mesh->vertices[p.index].connectFace(face_id); // For shadow volume, note connected face
  }

  this->subobject_length[this->subobject_no] += 3; // Increment vertex count
}

void Obj3DLoader::add_quad(std::vector<Vertex3D> face)
{
  // We can't render quads so split it into two triangles
  add_triangle(std::vector<Vertex3D>({face[0], face[1], face[2]}));
  add_triangle(std::vector<Vertex3D>({face[2], face[3], face[0]}));
}

