#include "ShadowVolume3D.h"

#include <iostream>

//#define DEBUG_TRACE_SHADOWS

ShadowVolume3D::ShadowVolume3D(Light3D* light, Mesh3D* mesh, Matrix4 model)
{
  //ctor
#ifdef DEBUG_TRACE_SHADOWS
  std::cout << "ShadowVolume3D " << this << " for mesh=" << mesh << " begin" << std::endl;
#endif
  this->light = light;
  this->mesh = mesh;
  this->inverse_model = model;
  this->inverse_model.invert();

  // Calculate the dot product of each face
  if (light->isPositional()) calc_dot_products_positional();
  if (light->isDirectional()) calc_dot_products_directional();

  // Find silouette edges
  find_silouette_edges();

  set_v(this->v.data(), this->v.size() / 3);

#ifdef DEBUG_TRACE_SHADOWS
  std::cout << "ShadowVolume3D " << this << " created" << std::endl;
#endif
}

ShadowVolume3D::~ShadowVolume3D()
{
  //dtor
#ifdef DEBUG_TRACE_SHADOWS
  std::cout << "ShadowVolume3D " << this << " destroyed" << std::endl;
#endif
}


void ShadowVolume3D::render(Matrix4 proj, Matrix4 view, Matrix4 model, GLenum face, ShaderProgram* shader)
{

  glUseProgram(shader->id());
  shader->setUniformMatrix4("projection", proj);
  shader->setUniformMatrix4("view", view);
  shader->setUniformMatrix4("model", model);

  bind_vao();
  initialize(shader);
  //glDrawArrays(GL_TRIANGLES, 0, this->count_v);
  glDrawArrays(GL_LINES, 0, this->count_v);
  unbind_vao();

}


void ShadowVolume3D::calc_dot_products_positional()
{
#ifdef DEBUG_TRACE_SHADOWS
  std::cout << "ShadowVolume3D::calc_dot_products_positional() " << this << std::endl;
  std::cout << "  mesh=" << this->mesh << std::endl;
  std::cout << "  light=" << this->light << std::endl;
#endif

  // Transform light position vector to model space coordinates (rotation * translation)
  Vector3 relative_light_pos = Vector4( this->inverse_model * Vector4(this->light->getPosition(), 1.0) ).xyz;

  for (auto& face : this->mesh->faces) {
    face.dot = face.normal.dot(Vector3(relative_light_pos - face.center).normalize());
  }
}


void ShadowVolume3D::calc_dot_products_directional()
{
#ifdef DEBUG_TRACE_SHADOWS
  std::cout << "ShadowVolume3D::calc_dot_products_directional() " << this << std::endl;
  std::cout << "  mesh=" << this->mesh << std::endl;
  std::cout << "  light=" << this->light << std::endl;
#endif

  // Transform light direction vector to model space coordinates (rotation only)
  Vector3 relative_light_dir = this->inverse_model.getRotationMatrix() * this->light->getDirection();

  for (auto& face : this->mesh->faces) {
    face.dot = face.normal.dot(relative_light_dir.normalize());
  }
}


void ShadowVolume3D::find_silouette_edges()
{
  for (auto& face : this->mesh->faces) {
    if (face.dot > 0) continue;
    for (int e=0; e<3; e++) {
      if (this->mesh->faces[ face.adjacent[e] ].dot > 0) {
        // polygon 'face' is lit, adjacent polygon 'face.adjacent[e]' is not
        Edge3D edge = face.edge(e);

        // Draw silouette edges
        this->v.insert( this->v.end(), edge.v1.data, edge.v1.data+3 );
        this->v.insert( this->v.end(), edge.v2.data, edge.v2.data+3 );

        // Draw lit faces
        //this->v.insert( this->v.end(), face.vertices[0].data, face.vertices[0].data+3 );
        //this->v.insert( this->v.end(), face.vertices[1].data, face.vertices[1].data+3 );
        //this->v.insert( this->v.end(), face.vertices[2].data, face.vertices[2].data+3 );

      }
    }
  }
}



