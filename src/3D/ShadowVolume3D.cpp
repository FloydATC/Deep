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

  if (light->isPositional()) compute_positional();
  if (light->isDirectional()) compute_directional();

  // Pass resulting vertex list to OpenGL
  set_v(this->v.data(), (int)this->v.size() / 4);

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


void ShadowVolume3D::renderShadow(Matrix4 proj, Matrix4 view, Matrix4 model, ShaderProgram* shader)
{
#ifdef DEBUG_TRACE_SHADOWS
  std::cout << "ShadowVolume3D::render() " << this << " projection:" << std::endl << proj << std::endl;
#endif
  glUseProgram(shader->id());
  shader->setUniformMatrix4("projection", proj);
  shader->setUniformMatrix4("view", view);
  shader->setUniformMatrix4("model", model);

  bind_vao();
  initialize(shader);
  glDrawArrays(GL_TRIANGLES, 0, this->count_v);

  //glDrawArrays(GL_LINES, 0, 6);
  //glDrawArrays(GL_TRIANGLES, 6, 3);

  unbind_vao();

}


void ShadowVolume3D::compute_positional()
{
#ifdef DEBUG_TRACE_SHADOWS
  std::cout << "ShadowVolume3D::compute_positional() " << this << std::endl;
  std::cout << "  mesh=" << this->mesh << std::endl;
  std::cout << "  light=" << this->light << std::endl;
#endif

  // Transform light position vector to model space coordinates (rotation * translation)
  Vector3 relative_light_pos = Vector4( this->inverse_model * Vector4(this->light->getPosition(), 1.0) ).xyz;

  // Compute dot product for each face
  for (auto& face : this->mesh->faces) {
    face.dot = face.normal.dot(Vector3(face.center - relative_light_pos).normalize());
  }

  // Find shared edges between lit and unlit faces
  for (auto& face : this->mesh->faces) {
    if (face.dot > 0) continue; // Only consider lit faces
    // Light cap
    std::vector<Vector4> light_cap;
    for (Vertex3D* vertex : face.vertices) {
      Vector4 v4 = Vector4( vertex->v, 1.0 );
      light_cap.push_back(v4);
      this->v.insert( this->v.end(), v4.data, v4.data+4 );
    }

    // Project light through each vertex
    std::vector<Vector3> light_dir;
    for (Vertex3D* vertex : face.vertices) light_dir.push_back( vertex->v - relative_light_pos );

    // Dark cap (reverse vertex order)
    std::vector<Vector4> dark_cap;
    dark_cap.resize(3);
    for (int i=(int)face.vertices.size()-1; i>=0; i--) {
      Vector4 v4 = Vector4( face.vertices[i]->v + light_dir[i], 0.0 ); // w = 0.0 = to infinity
      dark_cap[i] = v4;
      this->v.insert( this->v.end(), v4.data, v4.data+4 );
    }


    for (int e=0; e<3; e++) {
      if (face.adjacent[e] == -1) continue; // No adjacent polygon; mesh is not well-formed
      if (this->mesh->faces[ face.adjacent[e] ].dot > 0) {
        // polygon 'face' is lit, adjacent polygon is not
        uint8_t p1 = e;
        uint8_t p2 = (e + 1) % 3;

        this->v.insert( this->v.end(), light_cap[p2].data, light_cap[p2].data+4 );
        this->v.insert( this->v.end(), light_cap[p1].data, light_cap[p1].data+4 );
        this->v.insert( this->v.end(), dark_cap[p1].data, dark_cap[p1].data+4 );

        this->v.insert( this->v.end(), light_cap[p2].data, light_cap[p2].data+4 );
        this->v.insert( this->v.end(), dark_cap[p1].data, dark_cap[p1].data+4 );
        this->v.insert( this->v.end(), dark_cap[p2].data, dark_cap[p2].data+4 );

      }
    }
  }

}


void ShadowVolume3D::compute_directional()
{
#ifdef DEBUG_TRACE_SHADOWS
  std::cout << "ShadowVolume3D::compute_directional() " << this << std::endl;
  std::cout << "  mesh=" << this->mesh << std::endl;
  std::cout << "  light=" << this->light << std::endl;
#endif

  // Transform light direction vector to model space coordinates (rotation only)
  Vector3 relative_light_dir = this->inverse_model.getRotationMatrix() * this->light->getDirection();

  // Compute dot product for each face
  for (auto& face : this->mesh->faces) {
    face.dot = face.normal.dot(relative_light_dir.normalize());
  }
}


void ShadowVolume3D::set_v(float* v, int num_vertices)
{
  bind_vbo(this->vbo_v);
#ifdef DEBUG_TRACE_SHADOWS
  std::cout << "ShadowVolume3D::set_v() mesh=" << this << " vbo=" << this->vbo_v << " vertices=" << num_vertices << std::endl;
#endif
  glBufferData(GL_ARRAY_BUFFER, 4*num_vertices*sizeof(float), v, GL_STATIC_DRAW);
  this->count_v = num_vertices;
}



void ShadowVolume3D::initialize(ShaderProgram* shader)
{
#ifdef DEBUG_TRACE_SHADOWS
  std::cout << "ShadowVolume3D::initialize() mesh=" << this << " vao=" << this->vao << " name=" << this->name << std::endl;
#endif
  bind_vao();
  if (this->count_v  > 0 && shader->hasAttribute("attr_v")) {
#ifdef DEBUG_TRACE_SHADOWS
    std::cout << "ShadowVolume3D::initialize() enable V" << std::endl;
#endif
    bind_vbo(this->vbo_v);
    shader->enableAttribute("attr_v");
    shader->setAttributePointer("attr_v", 4, GL_FLOAT, sizeof(GLfloat), 4, 0);
  }
#ifdef DEBUG_TRACE_OPENGL
  glObjectLabel(GL_VERTEX_ARRAY, this->vao,    -1, "shadowVolume VAO");
  glObjectLabel(GL_BUFFER,       this->vbo_v,  -1, "shadowVolume VBO v");
#endif
  this->initialized = true;
}







