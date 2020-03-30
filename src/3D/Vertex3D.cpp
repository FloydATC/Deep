

#include "Vertex3D.h"

Vertex3D::Vertex3D(Vector3 position)
{
  this->v = position;
  this->index = 1234567890;
}


Vertex3D::~Vertex3D()
{

}


void Vertex3D::connectFace(uint32_t face_id)
{
  this->faces.push_back(face_id);
}


