#include "Material.h"

#include <iostream>

Material::Material()
{
  //ctor
  std::cout << "Material" << this << " created" << std::endl;
}

Material::~Material()
{
  //dtor
  std::cout << "Material" << this << " destroyed" << std::endl;
}


