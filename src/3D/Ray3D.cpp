#include "Ray3D.h"

#include <iostream>

//#define DEBUG_TRACE_RAY

Ray3D::Ray3D()
{
  //ctor
  this->finalized = false;
  this->name = "Ray3D";
  this->cast_shadow = false;
#ifdef DEBUG_TRACE_RAY
  std::cout << "Ray3D" << this << " created" << std::endl;
#endif
}

Ray3D::~Ray3D()
{
  //dtor
#ifdef DEBUG_TRACE_RAY
  std::cout << "Ray3D" << this << " destroyed" << std::endl;
#endif
}



void Ray3D::render(Matrix4 proj, Matrix4 view, Matrix4 model, Material* material, ShaderProgram* shader)
{
#ifdef DEBUG_TRACE_RAY
  std::cout << "Ray3D" << this << "::render()" << std::endl;
#endif
  // Resolve material + shader
  Material* use_material = my_material(material);
  ShaderProgram* use_shader = my_shader(shader);
  if (use_material == nullptr) std::cerr << this << " has no material" << std::endl;
  if (use_shader == nullptr) std::cerr << this << " has no shader" << std::endl;
  if (use_material == nullptr || use_shader == nullptr) return;

  // Set uniform values
  glUseProgram(use_shader->id());
  use_shader->setUniformMatrix4("projection", proj);
  use_shader->setUniformMatrix4("view", view);
  use_shader->setUniformMatrix4("model", model);
  //use_shader->setProjectionMatrix(proj);
  //use_shader->setViewMatrix(view);
  //use_shader->setModelMatrix(model);
  use_shader->setColors(use_material);

  bind_vao();
  glEnable(GL_DEPTH_TEST);
  if (this->finalized == false) finalize();
  if (this->initialized == false) initialize(shader);
  shader->setUniformBoolean("is_debug", false);
  shader->setUniformBoolean("use_texture", false);
  glLineWidth(1.0);
  glPointSize(4.0);
  glDrawArrays(GL_LINES, 0, 2);
  glDrawArrays(GL_POINTS, 0, 1);
  unbind_vao();
}


void Ray3D::finalize()
{
#ifdef DEBUG_TRACE_RAY
  std::cout << "Ray3D" << this << "::finalize()" << std::endl;
#endif
  float vertices[6] = {
    0.0, 0.0, 0.0,
    0.0, 0.0, 100.0
  };
  set_v(vertices, 2);
  this->finalized = true;
}
