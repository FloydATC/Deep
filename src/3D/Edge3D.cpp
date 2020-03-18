#include "Edge3D.h"

Edge3D::Edge3D(Vector3 v1, Vector3 v2)
{
  //ctor
  this->v1 = Vector4(v1, 1.0);
  this->v2 = Vector4(v2, 1.0);
}

Edge3D::Edge3D(Vector4 v1, Vector4 v2)
{
  //ctor
  this->v1 = v1;
  this->v2 = v2;
}

Edge3D::~Edge3D()
{
  //dtor
}
