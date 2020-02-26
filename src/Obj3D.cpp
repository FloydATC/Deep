#include "Obj3D.h"

#include <iostream>

Obj3D::Obj3D()
{
  //ctor
  glGenVertexArrays(1, &this->vao);
  glGenBuffers(1, &this->vbo_v);
  glGenBuffers(1, &this->vbo_vt);
  glGenBuffers(1, &this->vbo_vn);
  subobjects = 0;
  std::cout << "Obj3D " << this << " created" << std::endl;
}

Obj3D::~Obj3D()
{
  //dtor
  glDeleteBuffers(1, &this->vbo_v);
  glDeleteBuffers(1, &this->vbo_vt);
  glDeleteBuffers(1, &this->vbo_vn);
  glDeleteVertexArrays(1, &this->vao);
  for (auto& box : this->bounding_boxes) delete box;
  std::cout << "Obj3D " << this << " destroyed" << std::endl;
}


void Obj3D::render(int subobject)
{
  //std::cout << "Obj3D::render() subobject=" << subobject << std::endl;
  bind_vao();
  glDrawArrays(GL_TRIANGLES, subobject_start[subobject], subobject_length[subobject]);
  unbind_vao();
}





void Obj3D::set_subobjects(std::vector<int> start, std::vector<int> length)
{
  //std::cout << "Obj3D::set_subobjects()" << std::endl;
  subobject_start = start;
  subobject_length = length;
  subobjects = subobject_start.size();
  //for (int i=0; i<subobjects; i++) std::cout << "  i=" << i << " start=" << subobject_start[i] << " length=" << subobject_length[i] << std::endl;
  //std::cout << "Obj3D::set_subobjects() " << subobjects << " sub objects" << std::endl;
}


Box3D* Obj3D::bounding_box(int subobject)
{
  return bounding_boxes[subobject];
}


void Obj3D::set_bounding_boxes(std::vector<Box3D*> bounding_boxes)
{
  //std::cout << "Obj3D::set_bounding_box()" << std::endl;
  // Array with one or more Box3D pointers.
  this->bounding_boxes = bounding_boxes;
  for (auto& box : bounding_boxes) box->finalize();
}


void Obj3D::set_v(float* v, int length)
{
  bind_vbo(this->vbo_v);
  //std::cout << "Obj3D::set_v()" << std::endl;
  glBufferData(GL_ARRAY_BUFFER, 3*length*sizeof(float), v, GL_STATIC_DRAW);
  vertices = length;
}

void Obj3D::set_vt(float* vt, int length)
{
  bind_vbo(this->vbo_vt);
  //std::cout << "Obj3D::set_vt()" << std::endl;
  glBufferData(GL_ARRAY_BUFFER, 2*length*sizeof(float), vt, GL_STATIC_DRAW);
  vertices = length;
}

void Obj3D::set_vn(float* vn, int length)
{
  bind_vbo(this->vbo_vn);
  //std::cout << "Obj3D::set_vn()" << std::endl;
  glBufferData(GL_ARRAY_BUFFER, 3*length*sizeof(float), vn, GL_STATIC_DRAW);
  vertices = length;
}

void Obj3D::set_shader_v(GLint attr)
{
  shader_v = attr;
  if (attr == -1) return;
  bind_vbo(this->vbo_v);
  //std::cout << "Obj3D::set_shader_v() attr=" << attr << std::endl;
  glEnableVertexAttribArray(attr);
  glVertexAttribPointer(attr, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (char*) NULL);

  for (auto& box : bounding_boxes) box->set_shader_v(attr);
}

void Obj3D::set_shader_vt(GLint attr)
{
  shader_vt = attr;
  if (attr == -1) return;
  bind_vbo(this->vbo_vt);
  //std::cout << "Obj3D::set_shader_vt() attr=" << attr << std::endl;
  glEnableVertexAttribArray(attr);
  glVertexAttribPointer(attr, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (char*) NULL);
}

void Obj3D::set_shader_vn(GLint attr)
{
  shader_vn = attr;
  if (attr == -1) return;
  bind_vbo(this->vbo_vn);
  //std::cout << "Obj3D::set_shader_vn() attr=" << attr << std::endl;
  glEnableVertexAttribArray(attr);
  glVertexAttribPointer(attr, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (char*) NULL);
}

void Obj3D::bind_vao()
{
  glBindVertexArray(this->vao);
}

void Obj3D::unbind_vao()
{
  glBindVertexArray(0);
}


void Obj3D::bind_vbo(GLuint vbo)
{
  bind_vao();
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  //std::cout << "Obj3D VBO bound" << std::endl;
}


