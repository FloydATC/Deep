#include "Material.h"

#include <iostream>

//#define DEBUG_TRACE_MATERIAL

Material::Material()
{
  //ctor
  initialize();
#ifdef DEBUG_TRACE_MATERIAL
  std::cout << "Material created: " << this << std::endl;
#endif
}

Material::~Material()
{
  //dtor
#ifdef DEBUG_TRACE_MATERIAL
  std::cout << "Material destroyed" << std::endl;
#endif
}


void Material::initialize()
{
#ifdef DEBUG_TRACE_MATERIAL
  std::cout << "Material::initialize() material=" << this << " begin" << std::endl;
#endif
  setAmbientColor(1.0, 0.0, 1.0);
  setDiffuseColor(1.0, 0.0, 1.0);
  setSpecularColor(1.0, 0.0, 1.0);
  setEmissiveColor(1.0, 0.0, 1.0);

  this->specular_exponent   = 1.0;
  this->optical_density     = 1.0;
  this->opacity             = 1.0;
  this->illumination_mode     = 2; // See Material.h

  this->texture_ambient    = 0;
  this->texture_diffuse    = 0;
  this->texture_bump       = 0;
#ifdef DEBUG_TRACE_MATERIAL
  std::cout << "Material::initialize() material=" << this << " done" << std::endl;
#endif
}


Vector4 Material::getAmbientColor()
{
  return this->color_a;
}

Vector4 Material::getDiffuseColor()
{
  return this->color_d;
}

Vector4 Material::getSpecularColor()
{
  return this->color_s;
}

Vector4 Material::getEmissiveColor()
{
  return this->color_e;
}

void Material::setName(std::string name)
{
  this->name = name;
}


void Material::setFilename(std::string filename)
{
  this->file = filename;
}


void Material::setAmbientColor(float r, float g, float b)
{
#ifdef DEBUG_TRACE_MATERIAL
  std::cout << "Material::setAmbientColor() material=" << this << " r=" << r << " g=" << g << " b=" << b << std::endl;
#endif
  this->color_a.r = r;
  this->color_a.g = g;
  this->color_a.b = b;
  this->color_a.a = 1.0;
}


void Material::setDiffuseColor(float r, float g, float b)
{
#ifdef DEBUG_TRACE_MATERIAL
  std::cout << "Material::setDiffuseColor() material=" << this << " r=" << r << " g=" << g << " b=" << b << std::endl;
#endif
  this->color_d.r = r;
  this->color_d.g = g;
  this->color_d.b = b;
  this->color_d.a = 1.0;
}


void Material::setSpecularColor(float r, float g, float b)
{
#ifdef DEBUG_TRACE_MATERIAL
  std::cout << "Material::setSpecularColor() material=" << this << " r=" << r << " g=" << g << " b=" << b << std::endl;
#endif
  this->color_s.r = r;
  this->color_s.g = g;
  this->color_s.b = b;
  this->color_s.a = 1.0;
}


void Material::setEmissiveColor(float r, float g, float b)
{
#ifdef DEBUG_TRACE_MATERIAL
  std::cout << "Material::setEmissiveColor() material=" << this << " r=" << r << " g=" << g << " b=" << b << std::endl;
#endif
  this->color_e.r = r;
  this->color_e.g = g;
  this->color_e.b = b;
  this->color_e.a = 1.0;
}


std::ostream& operator <<(std::ostream& stream, const Material* material) {
  if (material == nullptr) {
    stream << "<no material>";
    return stream;
  } else {
    stream << "<Material " << material->file << ":" << material->name << ">";
    return stream;
  }
}


