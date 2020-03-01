#include "Mesh3D.h"

#include <iostream>

//#define DEBUG_TRACE_MESH

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
  this->name = "(untitled)";
  this->initialized = false; // False until shader attributes has been set
  this->texture = 0;
  this->texture_set = false;
  this->render_enabled = true;
  this->bounds = nullptr;
  this->debug = false;
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D" << this << " created" << std::endl;
#endif
}

Mesh3D::~Mesh3D()
{
  //dtor
  glDeleteBuffers(1, &this->vbo_v);
  glDeleteBuffers(1, &this->vbo_vt);
  glDeleteBuffers(1, &this->vbo_vn);
  glDeleteVertexArrays(1, &this->vao);
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D" << this << " destroyed" << std::endl;
#endif
}





void Mesh3D::initialize(ShaderProgram* shader)
{
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D" << this << "::initialize() vao=" << this->vao << " name=" << this->name << std::endl;
#endif
  bind_vao();
  if (this->count_v  > 0) {
#ifdef DEBUG_TRACE_MESH
    std::cout << "Mesh3D" << this << "::initialize() enable V" << std::endl;
#endif
    bind_vbo(this->vbo_v);
    shader->enableAttributeV();
    shader->setAttribPointerV(3, GL_FLOAT, sizeof(GLfloat), 3, 0);
  }
  if (this->count_vt  > 0) {
#ifdef DEBUG_TRACE_MESH
    std::cout << "Mesh3D" << this << "::initialize() enable VT" << std::endl;
#endif
    bind_vbo(this->vbo_vt);
    shader->enableAttributeVT();
    shader->setAttribPointerVT(2, GL_FLOAT, sizeof(GLfloat), 2, 0);
  }
  if (this->count_vn  > 0) {
#ifdef DEBUG_TRACE_MESH
    std::cout << "Mesh3D" << this << "::initialize() enable VN" << std::endl;
#endif
    bind_vbo(this->vbo_vn);
    shader->enableAttributeVN();
    shader->setAttribPointerVN(3, GL_FLOAT, sizeof(GLfloat), 3, 0);
  }
  this->initialized = true;
}


void Mesh3D::set_v(float* v, int num_vertices)
{
  bind_vbo(this->vbo_v);
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D" << this << "::set_v()" << std::endl;
#endif
  glBufferData(GL_ARRAY_BUFFER, 3*num_vertices*sizeof(float), v, GL_STATIC_DRAW);
  this->count_v = num_vertices;
}

void Mesh3D::set_vt(float* vt, int num_vertices)
{
  bind_vbo(this->vbo_vt);
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D" << this << "::set_vt()" << std::endl;
#endif
  glBufferData(GL_ARRAY_BUFFER, 2*num_vertices*sizeof(float), vt, GL_STATIC_DRAW);
  this->count_vt = num_vertices;
}

void Mesh3D::set_vn(float* vn, int num_vertices)
{
  bind_vbo(this->vbo_vn);
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D" << this << "::set_vn()" << std::endl;
#endif
  glBufferData(GL_ARRAY_BUFFER, 3*num_vertices*sizeof(float), vn, GL_STATIC_DRAW);
  this->count_vn = num_vertices;
}


void Mesh3D::bind_vao()
{
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D" << this << "::bind_vao()" << std::endl;
#endif
  glBindVertexArray(this->vao);
}

void Mesh3D::unbind_vao()
{
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D" << this << "::unbind_vao()" << std::endl;
#endif
  glBindVertexArray(0);
}


void Mesh3D::bind_vbo(GLuint vbo)
{
  bind_vao();
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D" << this << "::bind_vbo() " << vbo << std::endl;
#endif
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  //std::cout << "Mesh3D VBO bound" << std::endl;
}



void Mesh3D::setName(std::string name)
{
  this->name = name;
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D" << this << " named " << name << std::endl;
#endif
}


std::string Mesh3D::getName()
{
  return this->name;
}

void Mesh3D::setTexture(GLuint texture)
{
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D" << this << "::setTexture() name=" << this->name << " texture=" << texture << " enabled" << std::endl;
#endif
  this->texture = texture;
  this->texture_set = true;
}

void Mesh3D::setBounds(Mesh3D* box)
{
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D" << this << "::setBounds() name=" << this->name << " bounds set" << std::endl;
#endif
  if (bounds != nullptr) delete bounds;
  this->bounds = box;
}

Mesh3D* Mesh3D::getBounds()
{
  return this->bounds;
}

bool Mesh3D::isEnabled()
{
  return this->render_enabled;
}

void Mesh3D::show()
{
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D" << this << "::show() name=" << this->name << " render enabled" << std::endl;
#endif
  this->render_enabled = true;
}

void Mesh3D::hide()
{
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D" << this << "::hide() name=" << this->name << " render disabled" << std::endl;
#endif
  this->render_enabled = false;
}


void Mesh3D::setColor(float* color)
{
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D" << this << "::setColor()" << std::endl;
#endif
  this->color[0] = color[0];
  this->color[1] = color[1];
  this->color[2] = color[2];
  this->color[3] = color[3];
}


void Mesh3D::setColor(float r, float g, float b, float a)
{
#ifdef DEBUG_TRACE_MESH
  std::cout << "Mesh3D" << this << "::setColor()" << std::endl;
#endif
  this->color[0] = r;
  this->color[1] = g;
  this->color[2] = b;
  this->color[3] = a;
}



