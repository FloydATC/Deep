#include "3D/Obj3D.h"

#include <iostream>

Obj3D::Obj3D()
{
  //ctor
//  glGenVertexArrays(1, &this->vao);
//  glGenBuffers(1, &this->vbo_v);
//  glGenBuffers(1, &this->vbo_vt);
//  glGenBuffers(1, &this->vbo_vn);
  parts.clear();
  std::cout << "Obj3D " << this << " created" << std::endl;
}

Obj3D::~Obj3D()
{
  //dtor
  std::cout << "Obj3D " << this << " (" << name << ") destruction" << std::endl;
//  glDeleteBuffers(1, &this->vbo_v);
//  glDeleteBuffers(1, &this->vbo_vt);
//  glDeleteBuffers(1, &this->vbo_vn);
//  glDeleteVertexArrays(1, &this->vao);
  //for (auto& box : this->bounding_boxes) delete box;
//  std::cout << "Obj3D " << this << " " << name << " destruction complete" << std::endl;
}


void Obj3D::render(ShaderProgram* shader)
{
  //std::cout << "Obj3D::render() parts=" << this->parts.size() << std::endl;
  for (auto& part : this->parts) part->render(shader);
}



void Obj3D::addPart(Mesh3D* mesh)
{
  this->parts.push_back(mesh);
}

/*
Box3D* Obj3D::bounding_box(int subobject)
{
  return bounding_boxes[subobject];
}
*/
/*
void Obj3D::set_bounding_boxes(std::vector<Box3D*> bounding_boxes)
{
  //std::cout << "Obj3D::set_bounding_box()" << std::endl;
  // Array with one or more Box3D pointers.
  this->bounding_boxes = bounding_boxes;
  for (auto& box : bounding_boxes) box->finalize();
}
*/
/*
void Obj3D::set_v(float* v, int num_vertices)
{
  bind_vbo(this->vbo_v);
  //std::cout << "Obj3D::set_v()" << std::endl;
  glBufferData(GL_ARRAY_BUFFER, 3*num_vertices*sizeof(float), v, GL_STATIC_DRAW);
  vertices = num_vertices;
}

void Obj3D::set_vt(float* vt, int num_vertices)
{
  bind_vbo(this->vbo_vt);
  //std::cout << "Obj3D::set_vt()" << std::endl;
  glBufferData(GL_ARRAY_BUFFER, 2*num_vertices*sizeof(float), vt, GL_STATIC_DRAW);
  vertices = num_vertices;
}

void Obj3D::set_vn(float* vn, int num_vertices)
{
  bind_vbo(this->vbo_vn);
  //std::cout << "Obj3D::set_vn()" << std::endl;
  glBufferData(GL_ARRAY_BUFFER, 3*num_vertices*sizeof(float), vn, GL_STATIC_DRAW);
  vertices = num_vertices;
}
*/


