#include "Plane3D.h"

//#define DEBUG_TRACE_PLANE

Plane3D::Plane3D()
{
  //ctor
  this->finalized = false;
  this->name = "Plane3D";
#ifdef DEBUG_TRACE_PLANE
  std::cout << "Plane3D" << this << " created" << std::endl;
#endif
}

Plane3D::~Plane3D()
{
  //dtor
#ifdef DEBUG_TRACE_PLANE
  std::cout << "Plane3D" << this << " destroyed" << std::endl;
#endif
}


void Plane3D::render(ShaderProgram* shader)
{
#ifdef DEBUG_TRACE_PLANE
  std::cout << "Plane3D" << this << "::render()" << std::endl;
#endif
  bind_vao();
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  if (this->finalized == false) finalize();
  if (this->initialized == false) initialize(shader);
  shader->setColor(this->color);
  shader->setDebugFlag(true);
  shader->setTextureFlag(true);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glEnable(GL_CULL_FACE);
  unbind_vao();
#ifdef DEBUG_TRACE_PLANE
  std::cout << "Plane3D" << this << "::render() done" << std::endl;
#endif
}


void Plane3D::finalize()
{
#ifdef DEBUG_TRACE_PLANE
  std::cout << "Plane3D" << this << "::finalize()" << std::endl;
#endif
  float vertices_v[18] = {
   -10.0, 10.0, 0.0, // upper left
    10.0, 10.0, 0.0, // upper right
    10.0,-10.0, 0.0, // lower right
    10.0,-10.0, 0.0, // lower right
   -10.0,-10.0, 0.0, // lower left
   -10.0, 10.0, 0.0  // upper left
  };
  set_v(vertices_v, 6);
  float vertices_vt[18] = {
   -1.0, 1.0, // upper left
    1.0, 1.0, // upper right
    1.0,-1.0, // lower right
    1.0,-1.0, // lower right
   -1.0,-1.0, // lower left
   -1.0, 1.0  // upper left
  };
  set_vt(vertices_vt, 6);
  this->finalized = true;
}
