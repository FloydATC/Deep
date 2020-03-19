#include "Plane3D.h"

//#define DEBUG_TRACE_PLANE


Plane3D::Plane3D()
{
  //ctor
  this->finalized = false;
  this->name = "Plane3D";
  this->cast_shadow = false;
#ifdef DEBUG_TRACE_OPENGL
  glObjectLabel(GL_VERTEX_ARRAY, this->vao, -1, "Plane3D VAO");
  glObjectLabel(GL_BUFFER, this->vbo_v, -1, "Plane3D VBO v");
  glObjectLabel(GL_BUFFER, this->vbo_vt, -1, "Plane3D VBO vt");
  glObjectLabel(GL_BUFFER, this->vbo_vn, -1, "Plane3D VBO vn");
#endif
#ifdef DEBUG_TRACE_PLANE
  std::cout << "Plane3D created: " << this << std::endl;
#endif
}

Plane3D::~Plane3D()
{
  //dtor
#ifdef DEBUG_TRACE_PLANE
  std::cout << "Plane3D destroyed: " << this << std::endl;
#endif
}


void Plane3D::renderAmbient(Matrix4 proj, Matrix4 view, Matrix4 model, Material* material, ShaderProgram* shader)
{
#ifdef DEBUG_TRACE_PLANE
  std::cout << "Plane3D::render() mesh=" << this << " vao=" << this->vao << " vbo_t=" << this->vbo_v << " vbo_vt=" << this->vbo_vt << std::endl;
#endif
  // Resolve material + shader
  Material* use_material = my_material(material);
  ShaderProgram* use_shader = my_shader(shader);
  if (use_material == nullptr) std::cerr << this << " has no material" << std::endl;
  if (use_shader == nullptr) std::cerr << this << " has no shader" << std::endl;
  if (use_material == nullptr || use_shader == nullptr) return;
#ifdef DEBUG_TRACE_PLANE
  std::cout << "Plane3D::render() using material=" << use_material << " shader=" << use_shader << std::endl;
#endif

  // Set uniform values
  glUseProgram(use_shader->id());
  use_shader->setUniformMatrix4("projection", proj);
  use_shader->setUniformMatrix4("view", view);
  use_shader->setUniformMatrix4("model", model);
  use_shader->setUniformVector4("color_a", use_material->getAmbientColor());
  use_shader->setUniformVector4("color_d", Vector4(0,0,0,0));
  use_shader->setUniformVector4("color_s", Vector4(0,0,0,0));
  use_shader->setUniformVector4("color_e", Vector4(0,0,0,0));

  bind_vao();
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  if (this->finalized == false) finalize();
  if (this->initialized == false) initialize(use_shader);

  glBindTexture(GL_TEXTURE_2D, 0);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glEnable(GL_CULL_FACE);
  unbind_vao();
#ifdef DEBUG_TRACE_PLANE
  std::cout << "Plane3D::render() mesh=" << this << " done" << std::endl;
#endif
}


void Plane3D::renderLight(Matrix4 proj, Matrix4 view, Matrix4 model, Material* material, ShaderProgram* shader, Light3D* light)
{
#ifdef DEBUG_TRACE_PLANE
  std::cout << "Plane3D::render() mesh=" << this << " vao=" << this->vao << " vbo_t=" << this->vbo_v << " vbo_vt=" << this->vbo_vt << std::endl;
#endif
  // Resolve material + shader
  Material* use_material = my_material(material);
  ShaderProgram* use_shader = my_shader(shader);
  if (use_material == nullptr) std::cerr << this << " has no material" << std::endl;
  if (use_shader == nullptr) std::cerr << this << " has no shader" << std::endl;
  if (use_material == nullptr || use_shader == nullptr) return;
#ifdef DEBUG_TRACE_PLANE
  std::cout << "Plane3D::render() using material=" << use_material << " shader=" << use_shader << std::endl;
#endif

  // Set uniform values
  glUseProgram(use_shader->id());
  use_shader->setUniformMatrix4("projection", proj);
  use_shader->setUniformMatrix4("view", view);
  use_shader->setUniformMatrix4("model", model);
  use_shader->setColors(use_material);

  bind_vao();
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  if (this->finalized == false) finalize();
  if (this->initialized == false) initialize(use_shader);

  glBindTexture(GL_TEXTURE_2D, 0);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glEnable(GL_CULL_FACE);
  unbind_vao();
#ifdef DEBUG_TRACE_PLANE
  std::cout << "Plane3D::render() mesh=" << this << " done" << std::endl;
#endif
}


void Plane3D::finalize()
{
#ifdef DEBUG_TRACE_PLANE
  std::cout << "Plane3D::finalize() mesh=" << this << std::endl;
#endif
  float vertices_v[18] = {
   -10.0, 10.0, 0.0, // upper left
    10.0, 10.0, 0.0, // upper right
    10.0,-10.0, 0.0, // lower right
    10.0,-10.0, 0.0, // lower right
   -10.0,-10.0, 0.0, // lower left
   -10.0, 10.0, 0.0  // upper left
  };
  set_v(vertices_v, 6);
#ifdef DEBUG_TRACE_PLANE
  std::cout << "Plane3D::finalize() " << sizeof(vertices_v) << " bytes -> vbo_v " << this->vbo_v << std::endl;
#endif
  float vertices_vt[12] = {
   -1.0, 1.0, // upper left
    1.0, 1.0, // upper right
    1.0,-1.0, // lower right
    1.0,-1.0, // lower right
   -1.0,-1.0, // lower left
   -1.0, 1.0  // upper left
  };
  set_vt(vertices_vt, 6);
#ifdef DEBUG_TRACE_PLANE
  std::cout << "Plane3D::finalize() " << sizeof(vertices_vt) << " bytes -> vbo_vt " << this->vbo_vt << std::endl;
#endif
  float vertices_vn[18] = {
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0
  };
  set_vn(vertices_vn, 6);
#ifdef DEBUG_TRACE_PLANE
  std::cout << "Plane3D::finalize() " << sizeof(vertices_vn) << " bytes -> vbo_vn " << this->vbo_vn << std::endl;
#endif
  this->finalized = true;
}

