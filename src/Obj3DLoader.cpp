#include "Obj3DLoader.h"

#include <iostream>
#include <sstream>
#include <string>



Obj3DLoader::Obj3DLoader()
{
  //ctor
}

Obj3DLoader::~Obj3DLoader()
{
  //dtor
}

Obj3D* Obj3DLoader::load(std::string filename)
{
  std::cout << "Obj3DLoader::load() filename=" << filename << std::endl;
  scanner = new Obj3DScanner();
  scanner->load(filename);
  int errors = 0;


  //std::cout << "Obj3DLoader::load() begin parse loop" << std::endl;
  while (!scanner->is_eof()) {
    if (scanner->is_alpha()) { get_keyword(); continue; }

    //throw std::runtime_error ("Error parsing "+filename);
    errors++;
    scanner->advance();
  }
  if (errors > 0) {
    std::cerr << "Obj3DLoader::load() errors parsing " << filename << ": " << errors << std::endl;
  }

  delete scanner;
  this->filename = filename;
  //std::cout << "Obj3DLoader::load() produce object" << std::endl;

  Obj3D* object = new Obj3D();
  int sz = linear_points.size();
  float* v_array = make_v_array();
  float* vt_array = make_vt_array();
  float* vn_array = make_vn_array();
  object->set_v(v_array,   sz);
  object->set_vt(vt_array, sz);
  object->set_vn(vn_array, sz);
  free(v_array);
  free(vt_array);
  free(vn_array);
  object->set_subobjects(subobject_start, subobject_length);
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
    //std::cout << "Obj3DLoader::make_v_array()"
    //          << " x=" << p.v.x
    //          << " y=" << p.v.y
    //          << " z=" << p.v.z
    //          << std::endl;
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

/*
std::vector<float> Obj3DLoader::tokens_to_floats(std::vector<std::string> tokens)
{
  std::vector<float> result;
  std::string::size_type sz; // alias of size_t
  for(auto&& str: tokens) {
    std::cout << "str=" << str << " ";
    result.push_back(std::stof(str, &sz));

//  terminate called after throwing an instance of 'std::invalid_argument'
//  what():  stof

  }
  return result;
}
*/

void Obj3DLoader::get_keyword()
{
  std::string keyword = scanner->get_keyword();
  //std::cout << "Obj3DLoader::get_keyword() got keyword=" << keyword << std::endl;
  if (keyword == "mtllib") { get_mtllib(); return; }
  if (keyword == "usemtl") { get_usemtl(); return; }
  if (keyword == "f") { get_f(); return; }
  if (keyword == "o") { get_o(); return; }
  if (keyword == "s") { get_s(); return; }
  if (keyword == "v") { get_v(); return; }
  if (keyword == "vt") { get_vt(); return; }
  if (keyword == "vn") { get_vn(); return; }

  std::cout << "Obj3DLoader::get_keyword() unhandled keyword=" << keyword << std::endl;
}

void Obj3DLoader::get_mtllib()
{
  // Ignore for now
  while (!scanner->is_eof() && !scanner->is_eol()) { scanner->advance(); }
  scanner->consume('\n');
}

void Obj3DLoader::get_usemtl()
{
  // Ignore for now
  while (!scanner->is_eof() && !scanner->is_eol()) { scanner->advance(); }
  scanner->consume('\n');
}

void Obj3DLoader::get_f()
{
  // f 2563/2575/2561 2565/2576/2561 2566/2577/2561 2564/2578/2561
  std::vector<Point> face;
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
    Point p;
    p.v = indexed_v[v_no-1];
    p.vt = (vt_no==-1 ? Vector2(0.0f, 0.0f) : indexed_vt[vt_no-1]);
    p.vn = (vt_no==-1 ? Vector3(0.0f, 0.0f, 0.0f) : indexed_vn[vn_no-1]);
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
  scanner->consume('\n');
}

void Obj3DLoader::get_o()
{
  // Mark the beginning of sub-object
  subobject_start.push_back(linear_points.size());
  subobject_length.push_back(0);

  // Discard name
  while (!scanner->is_eof() && !scanner->is_eol()) { scanner->advance(); }
  scanner->consume('\n');
}

void Obj3DLoader::get_s()
{
  // Ignore for now
  while (!scanner->is_eof() && !scanner->is_eol()) { scanner->advance(); }
  scanner->consume('\n');
}

void Obj3DLoader::get_v()
{
  float x = scanner->get_float();
  float y = scanner->get_float();
  float z = scanner->get_float();
  indexed_v.push_back(Vector3(x, y, z));
  scanner->consume('\n');
}

void Obj3DLoader::get_vt()
{
  float u = scanner->get_float();
  float v = scanner->get_float();
  indexed_vt.push_back(Vector2(u, v));
  scanner->consume('\n');
}

void Obj3DLoader::get_vn()
{
  float x = scanner->get_float();
  float y = scanner->get_float();
  float z = scanner->get_float();
  indexed_vn.push_back(Vector3(x, y, z));
  scanner->consume('\n');
}

std::vector<Point> Obj3DLoader::compute_vn(std::vector<Point> face)
{
  std::cerr << "Obj3DLoader::compute_vn() not implemented yet" << std::endl;
  return face; // TODO: Compute vertex normals for this triangle
}

void Obj3DLoader::add_triangle(std::vector<Point> face)
{
  if (face[0].vn.x == 0.0 && face[0].vn.x == 0.0 && face[0].vn.x == 0.0) {
    face = compute_vn(face);
  }
  for (auto & p : face) {
    linear_points.push_back(p);
  }
  subobject_length[subobject_length.size()-1] += 3; // Increment vertex count
}

void Obj3DLoader::add_quad(std::vector<Point> face)
{
  // We can't render quads so split it into two triangles
  add_triangle(std::vector<Point>({face[0], face[1], face[2]}));
  add_triangle(std::vector<Point>({face[2], face[3], face[0]}));
}

