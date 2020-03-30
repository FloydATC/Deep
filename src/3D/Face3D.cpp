
#include "Face3D.h"

Face3D::Face3D(Vertex3D* v1, Vertex3D* v2, Vertex3D* v3)
{
  //ctor
  vertices.push_back(v1);
  vertices.push_back(v2);
  vertices.push_back(v3);

  calc_normal();
  calc_center();
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
  Vector3 ba = vertices[1]->v - vertices[0]->v;
  Vector3 ca = vertices[2]->v - vertices[0]->v;
  this->normal = ba.cross(ca).normalize();
  return this->normal;
}


Vector3 Face3D::calc_center()
{
  // Compute average position of the three vertices
  this->center = (vertices[0]->v + vertices[1]->v + vertices[2]->v) / 3.0;
  return this->center;
}


Edge3D Face3D::getEdge(uint8_t edge_no)
{
  return Edge3D(this->vertices[edge_no]->v, this->vertices[(edge_no+1)%3]->v);
}


void Face3D::setAdjacentFace(uint8_t edge_no, uint32_t face_id)
{
  this->adjacent[edge_no] = face_id;
}

