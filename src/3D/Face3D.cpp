#include "Face3D.h"

Face3D::Face3D(Vector3 v1, Vector3 v2, Vector3 v3)
{
  //ctor
  vertices.push_back(v1);
  vertices.push_back(v2);
  vertices.push_back(v3);
  calc_normal();
  adjacent = { -1, -1, -1 };
}

Face3D::~Face3D()
{
  //dtor
}


Vector3 Face3D::calc_normal()
{
  // Compute vertex normals for a flat surface
  //For each triangle ABC
  //  n := normalize(cross(B-A, C-A))
  //  A.n := n
  //  B.n := n
  //  C.n := n
  Vector3 ba = vertices[1] - vertices[0];
  Vector3 ca = vertices[2] - vertices[0];
  this->normal = ba.cross(ca).normalize();
  return this->normal;
}


Edge3D Face3D::edge(unsigned int index)
{
  return Edge3D(this->vertices[index], this->vertices[(index+1)%3]);
}
