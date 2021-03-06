#include "3D/Obj3D.h"

#include <iostream>

//#define DEBUG_TRACE_OBJ

Obj3D::Obj3D()
{
  //ctor
  parts.clear();
#ifdef DEBUG_TRACE_OBJ
  std::cout << "Obj3D created: " << this << std::endl;
#endif
}

Obj3D::~Obj3D()
{
  //dtor
#ifdef DEBUG_TRACE_OBJ
  std::cout << "Obj3D destroyed: " << this << std::endl;
#endif
}


void Obj3D::renderAmbient(Matrix4 proj, Matrix4 view, Matrix4 model, Material* material, ShaderProgram* shader)
{
#ifdef DEBUG_TRACE_OBJ
  std::cout << "Obj3D::renderAmbient() obj=" << this << " material=" << material << " shader=" << shader << std::endl;
#endif
  for (auto& part : this->parts) {
#ifdef DEBUG_TRACE_OBJ
    std::cout << "Obj3D::renderAmbient() part " << part << std::endl;
#endif

    if (part->isEnabled()) {
      part->bounds_enabled = this->bounds_enabled; // Propagate setting to subobject
      part->renderAmbient(proj, view, model, my_material(material), my_shader(shader));
    }
  }
}

void Obj3D::renderLight(Matrix4 proj, Matrix4 view, Matrix4 model, Material* material, ShaderProgram* shader, Light3D* light)
{
#ifdef DEBUG_TRACE_OBJ
  std::cout << "Obj3D::renderLight() obj=" << this << " material=" << material << " shader=" << shader << std::endl;
#endif
  for (auto& part : this->parts) {
#ifdef DEBUG_TRACE_OBJ
    std::cout << "Obj3D::renderLight() part " << part << std::endl;
#endif

    if (part->isEnabled()) {
      part->bounds_enabled = this->bounds_enabled; // Propagate setting to subobject
      part->renderLight(proj, view, model, my_material(material), my_shader(shader), light);
    }
  }
}

void Obj3D::addPart(Mesh3D* mesh)
{
#ifdef DEBUG_TRACE_OBJ
  std::cout << "Obj3D::addPart() obj=" << this << " added part " << mesh << std::endl;
#endif
  this->parts.push_back(mesh);
}


Mesh3D* Obj3D::getPart(int index)
{
  // Return nullptr if part number is invalid
  if ((int)this->parts.size() < index-1) {
    std::cerr << "Obj3D::getPart() obj=" << this << " part " << index << " not found" << std::endl;
    return nullptr;
  }
  return this->parts[index];
}


int Obj3D::numParts()
{
  return (int)this->parts.size();
}


void Obj3D::setTexture(GLuint texture)
{
  for (auto& part : this->parts) part->setTexture(texture);
}

void Obj3D::setShader(ShaderProgram* shader)
{
  for (auto& part : this->parts) part->setShader(shader);
}

void Obj3D::setDecalTexture(GLuint texture)
{
  for (auto& part : this->parts) part->setDecalTexture(texture);
}

void Obj3D::setDecalPosition(Vector2 position)
{
  for (auto& part : this->parts) part->setDecalPosition(position);
}


bool Obj3D::castsShadow()
{
  for (auto& part : this->parts) if (part->castsShadow()) return true;
  return false;
}


void Obj3D::generateShadowVolume(Light3D* light, Matrix4 model)
{
  for (auto& part : this->parts) if (part->castsShadow()) part->generateShadowVolume(light, model);
}


void Obj3D::renderShadowVolume(Matrix4 proj, Matrix4 view, Matrix4 model, ShaderProgram* shader)
{
  for (auto& part : this->parts) if (part->castsShadow()) part->renderShadowVolume(proj, view, model, shader);
}

void Obj3D::destroyShadowVolume()
{
  for (auto& part : this->parts) if (part->castsShadow()) part->destroyShadowVolume();
}

