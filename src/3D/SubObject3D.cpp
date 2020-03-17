#include "SubObject3D.h"

#include <iostream>

#include "3D/ShadowVolume3D.h"

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

      GLint texture_diffuse = glGetUniformLocation(use_shader->id(), "texture_diffuse");
      //std::cout << "SubObject3D::render() texture_diffuse=" << texture_diffuse << std::endl;
      int unit_diffuse = 0; // ???
      glUniform1i(texture_diffuse, unit_diffuse);
      glActiveTexture(GL_TEXTURE0 + unit_diffuse);
      glBindTexture(GL_TEXTURE_2D, this->texture);
      //glBindSampler(unit_diffuse, linearFiltering); // ?? see: genSamplers()

      GLint texture_decal = glGetUniformLocation(use_shader->id(), "texture_decal");
      //std::cout << "SubObject3D::render() texture_decal=" << texture_decal << std::endl;
      int unit_decal = 1; // ???
      glUniform1i(texture_decal, unit_decal);
      glActiveTexture(GL_TEXTURE0 + unit_decal);
      glBindTexture(GL_TEXTURE_2D, this->decal_texture);
      //glBindSampler(unit_diffuse, linearFiltering); // ?? see: genSamplers()

      if (use_shader->hasUniform("position_decal") && this->decal_position_set) {
        use_shader->setUniformVector2("position_decal", this->decal_position);
      }

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


void SubObject3D::generateShadowVolume(Light3D* light)
{
  this->shadow = new ShadowVolume3D(light, this);
}


void SubObject3D::destroyShadowVolume()
{
  delete this->shadow;
  this->shadow = nullptr;
}

