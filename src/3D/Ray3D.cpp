#include "Ray3D.h"

#include <iostream>

//#define DEBUG_TRACE_RAY

Ray3D::Ray3D()
{
  //ctor
  this->finalized = false;
  this->name = "Ray3D";
#ifdef DEBUG_TRACE_RAY
  std::cout << "Ray3D" << this << " created" << std::endl;
#endif
}

Ray3D::~Ray3D()
{
  //dtor
#ifdef DEBUG_TRACE_RAY
  std::cout << "Ray3D" << this << " destroyed" << std::endl;
#endif
}



void Ray3D::render(ShaderProgram* shader)
{
#ifdef DEBUG_TRACE_RAY
  std::cout << "Ray3D" << this << "::render()" << std::endl;
#endif
  bind_vao();
  glEnable(GL_DEPTH_TEST);
  if (this->finalized == false) finalize();
  if (this->initialized == false) initialize(shader);
  shader->setColor(this->color);
  shader->setDebugFlag(false);
  shader->setTextureFlag(false);
  glLineWidth(1.0);
  glPointSize(4.0);
  glDrawArrays(GL_LINES, 0, 2);
  glDrawArrays(GL_POINTS, 0, 1);
  unbind_vao();
}


void Ray3D::finalize()
{
#ifdef DEBUG_TRACE_RAY
  std::cout << "Ray3D" << this << "::finalize()" << std::endl;
#endif
  float vertices[6] = {
    0.0, 0.0, 0.0,
    0.0, 0.0, 10.0
  };
  set_v(vertices, 2);
  this->finalized = true;
}
