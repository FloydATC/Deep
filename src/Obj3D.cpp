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
}

Obj3D::~Obj3D()
{
  //dtor
  glDeleteBuffers(1, &this->vbo_v);
  glDeleteBuffers(1, &this->vbo_vt);
  glDeleteBuffers(1, &this->vbo_vn);
  glDeleteVertexArrays(1, &this->vao);
  std::cout << "Obj3D DESTROYED" << std::endl;
}

void Obj3D::set_subobjects(std::vector<int> start, std::vector<int> length)
{
  subobject_start = start;
  subobject_length = length;
  subobjects = subobject_start.size();
  std::cout << "Obj3D::set_subobjects() " << subobjects << " sub objects" << std::endl;
}

void Obj3D::set_v(float* v, int length)
{
  glBindVertexArray(this->vao);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo_v);
  glBufferData(GL_ARRAY_BUFFER, 3*length*sizeof(float), v, GL_STATIC_DRAW);
  vertices = length;
  check_gl("set_v");
}

void Obj3D::set_vt(float* vt, int length)
{
  glBindVertexArray(this->vao);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vt);
  glBufferData(GL_ARRAY_BUFFER, 2*length*sizeof(float), vt, GL_STATIC_DRAW);
  vertices = length;
  check_gl("set_vt");
}

void Obj3D::set_vn(float* vn, int length)
{
  glBindVertexArray(this->vao);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vn);
  glBufferData(GL_ARRAY_BUFFER, 3*length*sizeof(float), vn, GL_STATIC_DRAW);
  vertices = length;
  check_gl("set_vn");
}

void Obj3D::set_shader_v(GLint attr)
{
  //std::cout << "Obj3D::set_shader_v() attr=" << attr << std::endl;
  shader_v = attr;
  if (attr == -1) return;
  glBindVertexArray(this->vao);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo_v);
  glEnableVertexAttribArray(attr);
  glVertexAttribPointer(attr, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (char*) NULL);
  check_gl("set_shader_v");
}

void Obj3D::set_shader_vt(GLint attr)
{
  //std::cout << "Obj3D::set_shader_vt() attr=" << attr << std::endl;
  shader_vt = attr;
  if (attr == -1) return;
  glBindVertexArray(this->vao);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vt);
  glEnableVertexAttribArray(attr);
  glVertexAttribPointer(attr, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (char*) NULL);
  check_gl("set_shader_vt");
}

void Obj3D::set_shader_vn(GLint attr)
{
  //std::cout << "Obj3D::set_shader_vn() attr=" << attr << std::endl;
  shader_vn = attr;
  if (attr == -1) return;
  glBindVertexArray(this->vao);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vn);
  glEnableVertexAttribArray(attr);
  glVertexAttribPointer(attr, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (char*) NULL);
  check_gl("set_shader_vn");
}

void Obj3D::render(int subobject)
{
  glEnableVertexAttribArray(0);
  glBindVertexArray(this->vao);
  //std::cout << "Obj3D::render() start=" << subobject_start[subobject]
  //          << " length=" << subobject_length[subobject]
  //          << " total=" << vertices
  //          << " vao=" << vao
  //          << std::endl;
  //if (shader_v != -1) glEnableVertexAttribArray(shader_v);
  //if (shader_vt != -1) glEnableVertexAttribArray(shader_vt);
  //if (shader_vn != -1) glEnableVertexAttribArray(shader_vn);
  glDrawArrays(GL_TRIANGLES, subobject_start[subobject], subobject_length[subobject]);
  glBindVertexArray(0);
  check_gl("render");
}

void Obj3D::check_gl(std::string when)
{
  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    std::cerr << "Obj3D::" << when << "() OpenGL " << gluErrorString(err) << "(" << err << ")" << std::endl;
    //running = false;
  }
}
