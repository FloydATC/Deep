#include "Ray3D.h"

Ray3D::Ray3D()
{
  //ctor
  this->initialized = false;
}

Ray3D::~Ray3D()
{
  //dtor
}



void Ray3D::render(ShaderProgram* shader)
{
  //std::cout << "Ray3D::render()" << std::endl;
  bind_vao();
  glDisable(GL_DEPTH_TEST);
  if (this->finalized == false) finalize();
  if (this->initialized == false) initialize(shader);
  shader->setColor(this->color);
  shader->setDebugFlag(false);
  shader->setTextureFlag(false);
  glLineWidth(1.0);
  glPointSize(4.0);
  glDrawArrays(GL_LINES, 0, 2);
  glDrawArrays(GL_POINTS, 0, 1);
  glEnable(GL_DEPTH_TEST);
  unbind_vao();
}


void Ray3D::finalize()
{
  float vertices[6] = {
    0.0, 0.0, 0.0,
    0.0, 0.0, 10.0
  };
  set_v(vertices, 2);
  this->finalized = true;
}
