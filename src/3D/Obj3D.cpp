#include "3D/Obj3D.h"

#include <iostream>

Obj3D::Obj3D()
{
  //ctor
  parts.clear();
  std::cout << "Obj3D " << this << " created" << std::endl;
}

Obj3D::~Obj3D()
{
  //dtor
  std::cout << "Obj3D " << this << " (" << name << ") destruction" << std::endl;
}


void Obj3D::render(ShaderProgram* shader)
{
  //std::cout << "Obj3D::render() parts=" << this->parts.size() << std::endl;
  for (auto& part : this->parts) {
    if (part->isEnabled()) part->render(shader);
  }
}


void Obj3D::addPart(Mesh3D* mesh)
{
  this->parts.push_back(mesh);
}


Mesh3D* Obj3D::getPart(int index)
{
  // Return nullptr if part number is invalid
  return ((int)this->parts.size() >= index-1 ? this->parts[index] : nullptr);
}


int Obj3D::numParts()
{
  return this->parts.size();
}

