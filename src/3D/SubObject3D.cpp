#include "SubObject3D.h"

#include <iostream>

SubObject3D::SubObject3D()
{
  //ctor
  this->bounds = nullptr;
}

SubObject3D::~SubObject3D()
{
  //dtor
  if (bounds != nullptr) delete bounds;
}



void SubObject3D::render(ShaderProgram* shader)
{
  // Render mesh
  //std::cout << "SubObject3D::render() name=" << this->name << " vertices=" << this->count_v << std::endl;
  if (this->count_v > 0) {
    bind_vao();
    if (!this->initialized) this->initialize(shader);
    shader->setDebugFlag(true);
    shader->setTextureFlag(false);
    shader->setColor(0.5, 0.4, 0.4, 1.0);
    glDrawArrays(GL_TRIANGLES, 0, this->count_v);
    unbind_vao();
  }

  // Render bounding box
  if (this->bounds != nullptr) this->bounds->render(shader);
}

void SubObject3D::setBounds(Box3D* box)
{
  if (bounds != nullptr) delete bounds;
  this->bounds = box;
}




