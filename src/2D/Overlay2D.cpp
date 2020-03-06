#include "2D/Overlay2D.h"

#include <iostream>

Overlay2D::Overlay2D()
{
  //ctor
  glGenVertexArrays(1, &this->vao);
  glGenBuffers(1, &this->vbo);
#ifdef DEBUG_TRACE_OPENGL
  glObjectLabel(GL_VERTEX_ARRAY, this->vao, -1, "Overlay2D VAO");
  glObjectLabel(GL_BUFFER,       this->vbo, -1, "Overlay2D VBO");
#endif
  //std::cout << "Overlay2D() " << this << " created" << std::endl;
}

Overlay2D::~Overlay2D()
{
  //dtor
  glDeleteBuffers(1, &this->vbo);
  glDeleteVertexArrays(1, &this->vao);
  //std::cout << "Overlay2D() " << this << " destroyed" << std::endl;
}



void Overlay2D::pre_render()
{
  //std::cout << "Overlay2D::pre_render() glViewport(0, 0, " << this->width << ", " << this->height << ")" << std::endl;
  glViewport(0,0,this->width,this->height);
  //std::cout << "Overlay2D::pre_render() glUseProgram(" << this->shader->id() << ")" << std::endl;
  glUseProgram(this->shader->id());
  //std::cout << "Overlay2D::pre_render() glUniformMatrix4fv(" << this->shader->uniform_model_mat << ", 1, GL_FALSE, this->mat.get())" << std::endl;
  this->shader->setModelMatrix(this->mat);
  this->shader->setDebugFlag(false);
  this->shader->setTextureFlag(false);
  //glUniformMatrix4fv(this->shader->uniform_model_mat, 1, GL_FALSE, this->mat.get());
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  //std::cout << "Overlay2D::pre_render() done" << std::endl;
}

void Overlay2D::post_render()
{
  //std::cout << "Overlay2D::post_render()" << std::endl;

}


void Overlay2D::setShader(ShaderProgram* shader)
{
  //std::cout << "Overlay2D::setShader()" << std::endl;
  this->shader = shader;
}


void Overlay2D::setDimensions(int width, int height)
{
  //std::cout << "Overlay2D::setDimensions()" << std::endl;
  this->width = width;
  this->height = height;
  this->mat = Matrix4().Ortho(0, width, height, 0, -1, 1);
}


void Overlay2D::draw_polygon(Vector2 v1, Vector2 v2, Vector2 v3)
{
  //std::cout << "Overlay2D::draw_polygon()" << std::endl;
  Polygon2D* poly = new Polygon2D();
  pre_draw(poly);
  poly->draw(v1.x, v1.y, v2.x, v2.y, v3.x, v3.y);
  post_draw();
  delete poly;
}

void Overlay2D::draw_quad(Vector2 v1, Vector2 v2, Vector2 v3, Vector2 v4)
{
  //std::cout << "Overlay2D::draw_quad()" << std::endl;
  draw_polygon(v1, v2, v3);
  draw_polygon(v3, v2, v4);
}


void Overlay2D::pre_draw(Shape2D* shape)
{
  //std::cout << "Overlay2D::pre_draw()" << std::endl;
  //glUniform4fv(uniform_color, 1, this->color);
  this->shader->setColor(color);
  if (this->shader == nullptr) {
    std::cerr << "Overlay2D::pre_draw() shader not set" << std::endl;
    return;
  }
  shape->setShader(this->shader);
  shape->setDimensions(this->width, this->height);
  bind_vbo(this->vbo);
}

void Overlay2D::post_draw()
{
  //std::cout << "Overlay2D::post_draw()" << std::endl;

}


void Overlay2D::bind_vao()
{
  //std::cout << "Overlay2D::bind_vao() glBindVertexArray(" << this->vao << ")" << std::endl;
  glBindVertexArray(this->vao);
}


void Overlay2D::unbind_vao()
{
  //std::cout << "Overlay2D::unbind_vao() glBindVertexArray(0)" << std::endl;
  glBindVertexArray(0);
}


void Overlay2D::bind_vbo(GLuint vbo)
{
  bind_vao();
  //std::cout << "Overlay2D::bind_vbo() glBindBuffer(GL_ARRAY_BUFFER, " << vbo << ")" << std::endl;
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
}



