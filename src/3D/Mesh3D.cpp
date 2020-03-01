#include "Mesh3D.h"

#include <iostream>

Mesh3D::Mesh3D()
{
  //ctor
  glGenVertexArrays(1, &this->vao);
  glGenBuffers(1, &this->vbo_v);
  glGenBuffers(1, &this->vbo_vt);
  glGenBuffers(1, &this->vbo_vn);
  count_v = -1;
  count_vt = -1;
  count_vn = -1;
  name = "(untitled)";
  initialized = false; // False until shader attributes has been set
}

Mesh3D::~Mesh3D()
{
  //dtor
  glDeleteBuffers(1, &this->vbo_v);
  glDeleteBuffers(1, &this->vbo_vt);
  glDeleteBuffers(1, &this->vbo_vn);
  glDeleteVertexArrays(1, &this->vao);
}





void Mesh3D::initialize(ShaderProgram* shader)
{
  //std::cout << "Mesh3D" << this << "::initialize() vao=" << this->vao << " name=" << this->name << std::endl;
  bind_vao();
  if (this->count_v  > 0) {
    //std::cout << "Mesh3D" << this << "::initialize() enable V" << std::endl;
    bind_vbo(this->vbo_v);
    shader->enableAttributeV();
    shader->setAttribPointerV(3, GL_FLOAT, sizeof(GLfloat), 3, 0);
  }
  if (this->count_vt  > 0) {
    //std::cout << "Mesh3D" << this << "::initialize() enable VT" << std::endl;
    bind_vbo(this->vbo_vt);
    shader->enableAttributeVT();
    shader->setAttribPointerVT(2, GL_FLOAT, sizeof(GLfloat), 2, 0);
  }
  if (this->count_vn  > 0) {
    //std::cout << "Mesh3D" << this << "::initialize() enable VN" << std::endl;
    bind_vbo(this->vbo_vn);
    shader->enableAttributeVN();
    shader->setAttribPointerVN(3, GL_FLOAT, sizeof(GLfloat), 3, 0);
  }
  this->initialized = true;
}


void Mesh3D::set_v(float* v, int num_vertices)
{
  bind_vbo(this->vbo_v);
  //std::cout << "Obj3D::set_v()" << std::endl;
  glBufferData(GL_ARRAY_BUFFER, 3*num_vertices*sizeof(float), v, GL_STATIC_DRAW);
  this->count_v = num_vertices;
}

void Mesh3D::set_vt(float* vt, int num_vertices)
{
  bind_vbo(this->vbo_vt);
  //std::cout << "Obj3D::set_vt()" << std::endl;
  glBufferData(GL_ARRAY_BUFFER, 2*num_vertices*sizeof(float), vt, GL_STATIC_DRAW);
  this->count_vt = num_vertices;
}

void Mesh3D::set_vn(float* vn, int num_vertices)
{
  bind_vbo(this->vbo_vn);
  //std::cout << "Obj3D::set_vn()" << std::endl;
  glBufferData(GL_ARRAY_BUFFER, 3*num_vertices*sizeof(float), vn, GL_STATIC_DRAW);
  this->count_vn = num_vertices;
}


void Mesh3D::bind_vao()
{
  glBindVertexArray(this->vao);
}

void Mesh3D::unbind_vao()
{
  glBindVertexArray(0);
}


void Mesh3D::bind_vbo(GLuint vbo)
{
  bind_vao();
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  //std::cout << "Mesh3D VBO bound" << std::endl;
}



void Mesh3D::setName(std::string name)
{
  this->name = name;
  //std::cout << "Obj3D " << this << " named " << name << std::endl;
}


std::string Mesh3D::getName()
{
  return this->name;
}
