#include "Mesh3D.h"

#include <iostream>

//#define DEBUG_TRACE_MESH

int Mesh3D::mesh_serial_no = 0;

Mesh3D::Mesh3D()
{
  //ctor
  glGenVertexArrays(1, &this->vao);
  glGenBuffers(1, &this->vbo_v);
  glGenBuffers(1, &this->vbo_vt);
  glGenBuffers(1, &this->vbo_vn);
  this->count_v = -1;
  this->count_vt = -1;
  this->count_vn = -1;
  this->name = "(no name)";
  this->file = "(no file)";
  this->serial = ++mesh_serial_no;
  this->initialized = false; // False until shader attributes has been set
  this->texture = 0;
  this->texture_set = false;
  this->render_enabled = true;
  this->bounds = nullptr;
  this->bounds_enabled = false;
  this->debug = false;
  this->material = nullptr;
  this->shader = nullptr;
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D created: " << this << std::endl;
#endif
}

Mesh3D::~Mesh3D()
{
  //dtor
  glDeleteBuffers(1, &this->vbo_v);
  glDeleteBuffers(1, &this->vbo_vt);
  glDeleteBuffers(1, &this->vbo_vn);
  glDeleteVertexArrays(1, &this->vao);
  if (this->material != nullptr) delete this->material;
  if (this->bounds != nullptr) delete this->bounds;
  // Note: shader is owned by Scene3D; do not delete
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D destroyed: " << this << std::endl;
#endif
}





void Mesh3D::initialize(ShaderProgram* shader)
{
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D::initialize() mesh=" << this << " vao=" << this->vao << " name=" << this->name << std::endl;
#endif
  bind_vao();
  if (this->count_v  > 0 && shader->hasAttribute("attr_v")) {
#ifdef DEBUG_TRACE_MESH
    std::cout << "Mesh3D::initialize() enable V" << std::endl;
#endif
    bind_vbo(this->vbo_v);
    shader->enableAttribute("attr_v");
    shader->setAttributePointer("attr_v", 3, GL_FLOAT, sizeof(GLfloat), 3, 0);
  }
  if (this->count_vt  > 0 && shader->hasAttribute("attr_vt")) {
#ifdef DEBUG_TRACE_MESH
    std::cout << "Mesh3D::initialize() enable VT" << std::endl;
#endif
    bind_vbo(this->vbo_vt);
    shader->enableAttribute("attr_vt");
    shader->setAttributePointer("attr_vt", 2, GL_FLOAT, sizeof(GLfloat), 2, 0);
  }
  if (this->count_vn  > 0 && shader->hasAttribute("attr_vn")) {
#ifdef DEBUG_TRACE_MESH
    std::cout << "Mesh3D::initialize() enable VN" << std::endl;
#endif
    bind_vbo(this->vbo_vn);
    shader->enableAttribute("attr_vn");
    shader->setAttributePointer("attr_vn", 3, GL_FLOAT, sizeof(GLfloat), 3, 0);
  }
#ifdef DEBUG_TRACE_OPENGL
  glObjectLabel(GL_VERTEX_ARRAY, this->vao,    -1, std::string(this->name + " VAO").c_str());
  glObjectLabel(GL_BUFFER,       this->vbo_v,  -1, std::string(this->name + " VBO v").c_str());
  glObjectLabel(GL_BUFFER,       this->vbo_vt, -1, std::string(this->name + " VBO vt").c_str());
  glObjectLabel(GL_BUFFER,       this->vbo_vn, -1, std::string(this->name + " VBO vn").c_str());
#endif
  this->initialized = true;
}


void Mesh3D::set_v(float* v, int num_vertices)
{
  bind_vbo(this->vbo_v);
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D::set_v() mesh=" << this << " vbo=" << this->vbo_v << std::endl;
#endif
  glBufferData(GL_ARRAY_BUFFER, 3*num_vertices*sizeof(float), v, GL_STATIC_DRAW);
  this->count_v = num_vertices;
}

void Mesh3D::set_vt(float* vt, int num_vertices)
{
  bind_vbo(this->vbo_vt);
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D::set_vt() mesh=" << this << " vbo=" << this->vbo_vt << std::endl;
#endif
  glBufferData(GL_ARRAY_BUFFER, 2*num_vertices*sizeof(float), vt, GL_STATIC_DRAW);
  this->count_vt = num_vertices;
}

void Mesh3D::set_vn(float* vn, int num_vertices)
{
  bind_vbo(this->vbo_vn);
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D::set_vn() mesh=" << this << " vbo=" << this->vbo_vt << std::endl;
#endif
  glBufferData(GL_ARRAY_BUFFER, 3*num_vertices*sizeof(float), vn, GL_STATIC_DRAW);
  this->count_vn = num_vertices;
}


void Mesh3D::bind_vao()
{
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D::bind_vao() mesh=" << this << std::endl;
#endif
  glBindVertexArray(this->vao);
}

void Mesh3D::unbind_vao()
{
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D::unbind_vao() mesh=" << this << std::endl;
#endif
  glBindVertexArray(0);
}


void Mesh3D::bind_vbo(GLuint vbo)
{
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D::bind_vbo() mesh=" << this << " vbo=" << vbo << std::endl;
#endif
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  //std::cout << "Mesh3D VBO bound" << std::endl;
}



void Mesh3D::setName(std::string name)
{
  this->name = name;
}


std::string Mesh3D::getName()
{
  return this->name;
}

void Mesh3D::setFilename(std::string filename)
{
  this->file = filename;
}


std::string Mesh3D::getFilename()
{
  return this->file;
}

// Use material assigned to this Mesh3D if set, otherwise use default
Material* Mesh3D::my_material(Material* standard)
{
  return (this->material != nullptr ? this->material : standard);
}


// Use shader assigned to this Mesh3D if set, otherwise use default
ShaderProgram* Mesh3D::my_shader(ShaderProgram* standard)
{
  return (this->shader != nullptr ? this->shader : standard);
}


void Mesh3D::setTexture(GLuint texture)
{
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D::setTexture() mesh=" << this << " name=" << this->name << " texture=" << texture << " enabled" << std::endl;
#endif
  this->texture = texture;
  this->texture_set = true;
}

void Mesh3D::setBounds(Mesh3D* box)
{
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D::setBounds() mesh=" << this << " bounds set" << std::endl;
#endif
  if (this->bounds != nullptr) delete this->bounds;
  this->bounds = box;
}

void Mesh3D::setMaterial(Material* material)
{
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D::setMaterial() mesh=" << this << " material=" << material << std::endl;
#endif
  if (this->material != nullptr) delete this->material;
  this->material = material;
}

void Mesh3D::setShader(ShaderProgram* shader)
{
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D::setShader() mesh=" << this << " shader=" << shader << std::endl;
#endif
  // Note: shaders are owned by the Scene3D; do not delete
  this->shader = shader;
}

Mesh3D* Mesh3D::getBounds()
{
  return this->bounds;
}

Material* Mesh3D::getMaterial()
{
  return this->material;
}

ShaderProgram* Mesh3D::getShader()
{
  return this->shader;
}



bool Mesh3D::isEnabled()
{
  return this->render_enabled;
}

void Mesh3D::show()
{
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D::show() mesh=" << this << " name=" << this->name << " render enabled" << std::endl;
#endif
  this->render_enabled = true;
}

void Mesh3D::hide()
{
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D::hide() mesh=" << this << " name=" << this->name << " render disabled" << std::endl;
#endif
  this->render_enabled = false;
}


void Mesh3D::setDecalTexture(GLuint texture)
{
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D::setDecalTexture() mesh=" << this << " decal texture set" << std::endl;
#endif
  this->decal_texture = texture;
  this->decal_texture_set = true;
}


void Mesh3D::setDecalPosition(Vector2 position)
{
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D::setDecalPosition() mesh=" << this << " decal position set" << std::endl;
#endif
  this->decal_position = position;
  this->decal_position_set = true;
}


std::ostream& operator <<(std::ostream& stream, const Mesh3D* mesh) {
  if (mesh == nullptr) {
    stream << "<no mesh>";
    return stream;
  } else {
    stream << "<Mesh3D #" << mesh->serial << " " << mesh->file << ":" << mesh->name << ">";
    return stream;
  }
}


