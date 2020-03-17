#include "ShadowVolume3D.h"

#include <iostream>

//#define DEBUG_TRACE_SHADOWS

ShadowVolume3D::ShadowVolume3D(Light3D* light, Mesh3D* mesh)
{
  //ctor
#ifdef DEBUG_TRACE_SHADOWS
  std::cout << "ShadowVolume3D " << this << " for mesh=" << mesh << " begin" << std::endl;
#endif
  this->light = light;
  this->mesh = mesh;
  if (light->isPositional()) calc_dot_products_positional();
  if (light->isDirectional()) calc_dot_products_directional();
#ifdef DEBUG_TRACE_SHADOWS
  std::cout << "ShadowVolume3D " << this << " created" << std::endl;
#endif
}

ShadowVolume3D::~ShadowVolume3D()
{
  //dtor
#ifdef DEBUG_TRACE_SHADOWS
  std::cout << "ShadowVolume3D " << this << " destroyed" << std::endl;
#endif
}


void ShadowVolume3D::render(Matrix4 proj, Matrix4 view, Matrix4 model, Material* material, ShaderProgram* shader)
{

}


void ShadowVolume3D::calc_dot_products_positional()
{
  std::cout << "ShadowVolume3D::calc_dot_products_positional() " << this << std::endl;
  std::cout << "  mesh=" << this->mesh << std::endl;
  std::cout << "  light=" << this->light << std::endl;


}


void ShadowVolume3D::calc_dot_products_directional()
{

}
