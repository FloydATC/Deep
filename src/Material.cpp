#include "Material.h"

#include <iostream>

Material::Material()
{
  //ctor
  initialize();
  std::cout << "Material" << this << " created" << std::endl;
}

Material::~Material()
{
  //dtor
  std::cout << "Material" << this << " destroyed" << std::endl;
}


void Material::initialize()
{
  Vector3 color_ambient     = Vector3(0.1, 0.1, 0.1);
  Vector3 color_diffuse     = Vector3(1.0, 1.0, 1.0);
  Vector3 color_specular    = Vector3(1.0, 1.0, 1.0);
  Vector3 color_emissive    = Vector3(0.0, 0.0, 0.0);
  float specular_exponent   = 1.0;
  float optical_density     = 1.0;
  float opacity             = 1.0;
  int illumination_mode     = 2; // See Material.h

  GLuint texture_ambient    = 0;
  GLuint texture_diffuse    = 0;
  GLuint texture_bump       = 0;
}

