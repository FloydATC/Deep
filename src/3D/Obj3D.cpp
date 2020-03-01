#include "3D/Obj3D.h"

#include <iostream>

//#define DEBUG_TRACE_OBJ

Obj3D::Obj3D()
{
  //ctor
  parts.clear();
#ifdef DEBUG_TRACE_OBJ
  std::cout << "Obj3D " << this << " created" << std::endl;
#endif
}

Obj3D::~Obj3D()
{
  //dtor
#ifdef DEBUG_TRACE_OBJ
  std::cout << "Obj3D " << this << " (" << name << ") destruction" << std::endl;
#endif
}


void Obj3D::render(ShaderProgram* shader)
{
#ifdef DEBUG_TRACE_OBJ
  std::cout << "Obj3D" << this << "::render() name=" << this->name << " parts=" << this->parts.size() << std::endl;
#endif
  for (auto& part : this->parts) {
#ifdef DEBUG_TRACE_OBJ
    std::cout << "Obj3D" << this << "::render() part name=" << part->getName() << std::endl;
#endif
    if (part->isEnabled()) part->render(shader);
  }
}

void Obj3D::addPart(Mesh3D* mesh)
{
#ifdef DEBUG_TRACE_OBJ
  std::cout << "Obj3D" << this << "::addPart() mesh=" << mesh << " name=" << mesh->getName() << std::endl;
#endif
  this->parts.push_back(mesh);
}


Mesh3D* Obj3D::getPart(int index)
{
  // Return nullptr if part number is invalid
  if ((int)this->parts.size() < index-1) {
    std::cerr << "Obj3D" << this << "::getPart() part " << index << " not found" << std::endl;
    return nullptr;
  }
  return this->parts[index];
}


int Obj3D::numParts()
{
  return this->parts.size();
}



