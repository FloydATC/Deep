#include "2D/Shape2D.h"

#include <iostream>


Shape2D::Shape2D()
{
  //ctor
}

Shape2D::~Shape2D()
{
  //dtor
}


void Shape2D::draw_untextured_vbo(GLsizeiptr arrsize, const void* arr, GLenum type, GLsizei typesize, GLenum mode, GLsizei vertices)
{
  glBufferData(GL_ARRAY_BUFFER, arrsize, arr, GL_STREAM_DRAW); // Buffer will be used only once
  glEnableVertexAttribArray(shader->vertex_v);
  glDisableVertexAttribArray(shader->vertex_vt);
  glVertexAttribPointer(shader->vertex_v, 2, type, GL_FALSE, 2*typesize, (char*) NULL+0*typesize);
  glDrawArrays(mode, 0, vertices);
}


void Shape2D::setShader(ShaderProgram* shader)
{
  this->shader = shader;
}


void Shape2D::setDimensions(int width, int height)
{
  // Shape needs to know the display size so it can do any necessary transforms
  this->width = width;
  this->height = height;
}


int Shape2D::tx(int x)
{
  return x; // Transform coordinate as needed
}

int Shape2D::ty(int y)
{
  //std::cout << "Shape2D::ty() height=" << this->height << " - y=" << y << " = " << (this->height - y) << std::endl;
  //return this->height - y; // Transform coordinate as needed
  return y;
}

