#include "SubObject3D.h"

#include <iostream>

//#define DEBUG_TRACE_SUBOBJ

SubObject3D::SubObject3D()
{
  //ctor
  this->bounds = nullptr;
}

SubObject3D::~SubObject3D()
{
  //dtor
  if (bounds != nullptr) delete bounds;
}



void SubObject3D::render(Matrix4 proj, Matrix4 view, Matrix4 model, Material* material, ShaderProgram* shader)
{
  // Resolve material + shader
  Material* use_material = my_material(material);
  ShaderProgram* use_shader = my_shader(shader);
  if (use_material == nullptr) std::cerr << this << " has no material" << std::endl;
  if (use_shader == nullptr) std::cerr << this << " has no shader" << std::endl;
#ifdef DEBUG_TRACE_SUBOBJ
  std::cout << "SubObject3D::render() mesh=" << this << " material=" << use_material << " shader=" << use_shader << std::endl;
#endif

  if (this->count_v > 0 && use_shader != nullptr && use_material != nullptr) {
#ifdef DEBUG_TRACE_SUBOBJ
  std::cout << "SubObject3D::render() vertices:" << this->count_v << " textured:" << (this->texture_set ? "Yes" : "No") << std::endl;
#endif

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
    if (!this->initialized) this->initialize(use_shader);
    use_shader->setUniformBoolean("is_debug", this->debug);

    if (this->texture_set) {
      use_shader->setUniformBoolean("use_texture", true);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, this->texture);
    } else {
      use_shader->setUniformBoolean("use_texture", false);
    }

    glDrawArrays(GL_TRIANGLES, 0, this->count_v);

    unbind_vao();
  }

  // Render bounding box
  if (this->bounds != nullptr && this->bounds_enabled) {
    this->bounds->render(proj, view, model, use_material, use_shader);
  }
}




