#include "Box3D.h"

#include <initializer_list>

Box3D::Box3D()
{
  //ctor
  this->vertices.resize(8*3); // 8 corners, 3 dimensions (x,y,z)
  glGenVertexArrays(1, &this->vao);
  glGenBuffers(1, &this->vbo);
  glGenBuffers(1, &this->ibo);
  //std::cout << "Box3D " << this << " created" << std::endl;
}

Box3D::~Box3D()
{
  //dtor
  glDeleteBuffers(1, &this->ibo);
  glDeleteBuffers(1, &this->vbo);
  glDeleteVertexArrays(1, &this->vao);
  //std::cout << "Box3D " << this << " destroyed" << std::endl;
}


void Box3D::render()
{
  bind_vao();
  //bind_ibo(this->ibo); // Intel drivers do not save ELEMENT_ARRAY_BUFFER_BINDING?
  //bind_vbo(this->vbo);
  glLineWidth(1.0);
  glPointSize(2.0);
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


void Box3D::set_shader_v(GLint attr)
{
  shader_v = attr;

  bind_vbo(this->vbo);
  //std::cout << "Obj3D::set_shader_v() glEnableVertexAttribArray(attr=" << attr << ")" << std::endl;
  glEnableVertexAttribArray(attr);
  //std::cout << "Obj3D::set_shader_v() glVertexAttribPointer(attr=" << attr << ", 3, GL_FLOAT, GL_FALSE, " << 3*sizeof(GLfloat) << ", (char*) NULL)" << std::endl;
  glVertexAttribPointer(attr, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (char*) NULL);
}


void Box3D::set_v()
{
  // Define the eight corner vertices of a box
  for (int i=0; i<8; i++) {
    vertices[i*3+0] = ((i&0b100)==0 ? minimum.x : maximum.x);
    vertices[i*3+1] = ((i&0b010)==0 ? minimum.y : maximum.y);
    vertices[i*3+2] = ((i&0b001)==0 ? minimum.z : maximum.z);
    //std::cout << "  " << i << ": " << vertices[i*3+0] << ", " << vertices[i*3+1] << ", " << vertices[i*3+2] << std::endl;
  }

  bind_vbo(this->vbo);
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


void Box3D::bind_vao()
{
  //std::cout << "Box3D::bind_vao() glBindVertexArray(" << this->vao << ")" << std::endl;
  glBindVertexArray(this->vao);
}


void Box3D::unbind_vao()
{
  //std::cout << "Box3D::unbind_vao() glBindVertexArray(0)" << std::endl;
  glBindVertexArray(0);
}


void Box3D::bind_vbo(GLuint vbo)
{
  bind_vao();
  //std::cout << "Box3D::bind_vbo() glBindBuffer(GL_ARRAY_BUFFER, " << vbo << ")" << std::endl;
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
}


void Box3D::bind_ibo(GLuint ibo)
{
  bind_vao();
  //std::cout << "Box3D::bind_ibo() glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, " << ibo << ")" << std::endl;
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
}



