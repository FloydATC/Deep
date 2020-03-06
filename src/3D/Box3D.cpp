#include "3D/Box3D.h"

#include <initializer_list>

Box3D::Box3D()
{
  //ctor
  this->vertices.resize(8*3); // 8 corners, 3 dimensions (x,y,z)
  glGenBuffers(1, &this->ibo);
#ifdef DEBUG_TRACE_OPENGL
  glObjectLabel(GL_BUFFER, this->ibo, -1, "Box3D IBO");
#endif
  //std::cout << "Box3D " << this << " created" << std::endl;
}

Box3D::~Box3D()
{
  //dtor
  //std::cout << "Box3D " << this << " " << name << " destruction" << std::endl;
  glDeleteBuffers(1, &this->ibo);
  //std::cout << "Box3D " << this << " " << name << " destruction complete" << std::endl;
}


void Box3D::render(ShaderProgram* shader)
{
  //std::cout << "Box3D" << this << "::render()" << std::endl;
  bind_vao();
  if (!this->finalized) this->finalize();
  if (!this->initialized) this->initialize(shader);
  shader->setColor(1.0, 1.0, 0.0, 1.0);
  shader->setDebugFlag(false);
  shader->setTextureFlag(false);
  glLineWidth(1.0);
  glPointSize(4.0);
  //std::cout << "Box3D::render() glDrawArrays(GL_POINTS, 0, 12)" << std::endl;
  glDrawArrays(GL_POINTS, 0, 8);
  //std::cout << "Box3D::render() glDrawElements(GL_LINES, " << index.size() << ", GL_UNSIGNED_INT, 0)" << std::endl;
  glDrawElements(GL_LINES, index.size(), GL_UNSIGNED_INT, 0); // Indexed draw
  unbind_vao();
}


void Box3D::finalize()
{
  //std::cout << "Box3D::finalize()" << std::endl;
  this->set_v();
  this->set_i();
  this->finalized = true;
}


void Box3D::set_v()
{
  // Define the eight corner vertices of a box
  for (int i=0; i<8; i++) {
    vertices[i*3+0] = ((i&0b001)==0 ? minimum.x : maximum.x);
    vertices[i*3+1] = ((i&0b010)==0 ? minimum.y : maximum.y);
    vertices[i*3+2] = ((i&0b100)==0 ? minimum.z : maximum.z);
    //std::cout << "  " << i << ": " << vertices[i*3+0] << ", " << vertices[i*3+1] << ", " << vertices[i*3+2] << std::endl;
  }
  this->count_v = 8;

  bind_vbo(this->vbo_v);
  //std::cout << "Box3D::set_v() glBufferData(GL_ARRAY_BUFFER, " << sizeof(GLfloat)*vertices.size() << ", vertices.data(), GL_STATIC_DRAW)" << std::endl;
  // Box3D::set_v() glBufferData(GL_ARRAY_BUFFER, 0, vertices.data(), GL_STATIC_DRAW)
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
}


void Box3D::set_i()
{
  // Set vertex index to draw 12 edges of a box

  //std::cout << "index:";
  //for (auto& i: index) std::cout << " " << i;
  //std::cout << std::endl;

  bind_ibo(this->ibo);
  //std::cout << "Box3D::set_i() glBufferData(GL_ELEMENT_ARRAY_BUFFER, " << sizeof(GLuint)*index.size() << ", index.data(), GL_STATIC_DRAW)" << std::endl;
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*index.size(), index.data(), GL_STATIC_DRAW);
}




void Box3D::extend(Vector3 vertex)
{
  if (first) {
    minimum = vertex;
    maximum = vertex;
    first = false;
  } else {
    if (vertex.x < minimum.x) minimum.x = vertex.x;
    if (vertex.x > maximum.x) maximum.x = vertex.x;
    if (vertex.y < minimum.y) minimum.y = vertex.y;
    if (vertex.y > maximum.y) maximum.y = vertex.y;
    if (vertex.z < minimum.z) minimum.z = vertex.z;
    if (vertex.z > maximum.z) maximum.z = vertex.z;
  }
}


void Box3D::bind_ibo(GLuint ibo)
{
  bind_vao();
  //std::cout << "Box3D::bind_ibo() glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, " << ibo << ")" << std::endl;
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
}



