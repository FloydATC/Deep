#include "3D/Prop3D.h"

#include "Matrices.h"


Prop3D::Prop3D()
{
  //ctor
  this->object = nullptr;
  this->shader = nullptr;
  this->pos = Vector3(0.0, 0.0, 0.0);
  this->dir = Vector3(0.0, 0.0, 0.0);
  this->scale = Vector3(1.0, 1.0, 1.0);
  this->need_recalc = true;
  this->xy_plane.resize(4); // 4 x Vector2
}

Prop3D::~Prop3D()
{
  //dtor
}


void Prop3D::render(Camera3D camera) {
  if (this->need_recalc) this->recalculate_matrix();

  glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
  //std::cout << "Prop3D" << this << "::render() glBindTexture(GL_TEXTURE_2D, " << this->texture << ")" << std::endl;
  glBindTexture(GL_TEXTURE_2D, this->texture);

  glUseProgram(this->shader->id());
  glUniformMatrix4fv(this->shader->uniform_camera_mat, 1, GL_FALSE, camera.matrix().get());
  glUniformMatrix4fv(this->shader->uniform_model_mat, 1, GL_FALSE, this->mat.get());

  glUniform1i(this->shader->uniform_debug_flag, 0);
  for (int i=0; i<this->object->subobjects; i++) {
    this->object->render(i); // Render Obj3D subobject i
  }

  glBindTexture(GL_TEXTURE_2D, 0);

  // Render bounding boxes for debugging
#ifdef DEBUG_RENDER_BOUNDING_BOXES
  glDisable(GL_DEPTH_TEST);
  if (true) {
    glUniform1i(this->shader->uniform_debug_flag, 1);
    for (int i=0; i<this->object->subobjects; i++) {
      this->object->bounding_box(i)->render(); // Render bounding box of Obj3D subobject i
    }
  }
  glEnable(GL_DEPTH_TEST);
#endif

  recalculate_xy_plane(camera);
}


void Prop3D::recalculate_matrix()
{
  //std::cout << "Prop3D::recalculate() begin" << std::endl;
  this->mat = Matrix4().translate(this->pos); // Relative position
  this->mat *= Matrix4().rotate(this->dir.z, Vector3(0.0f, 0.0f, 1.0f)); // Rotation angle and axis
  this->mat *= Matrix4().rotate(this->dir.y, Vector3(0.0f, 1.0f, 0.0f)); // Rotation angle and axis
  this->mat *= Matrix4().rotate(this->dir.x, Vector3(1.0f, 0.0f, 0.0f)); // Rotation angle and axis
  this->mat *= Matrix4().scale(this->scale.x, this->scale.y, this->scale.z);
  //std::cout << "Prop3D::recalculate() done" << std::endl;
  this->need_recalc = false;
}


Vector3 Prop3D::project(Vector3 v3, Camera3D camera)
{
  // This function is used to project (transform) each of the four xy_plane corners
  // from world coordinates onto the screen. We will use this 2-dimensional plane
  // to pinpoint mouse events relative to the 3D object.

  // We need a Vector4 for the projection since the matrices are 4x4 and use perspective
  Vector4 v4 =  camera.matrix() * this->mat * Vector4(v3.x, v3.y, v3.z, 1.0);

  // Return 2D display coordinates + depth component
  int w2 = camera.getWidth() / 2;
  int h2 = camera.getHeight() / 2;
  return Vector3(w2+(v4.x*w2/v4.w), h2+(v4.y*-h2/v4.w), v4.z/v4.w); // flip Y so 0,0 is upper left corner
}


void Prop3D::recalculate_xy_plane(Camera3D camera)
{
  // This function is called right after we render the prop in 3D,
  // as we then have both the camera and model matrix available.
  // The Obj3D size must be (roughly) 1.0 and it must be (roughly) centered at 0,0,0

  // Calculate 4 corners of Obj3D XY plane
  this->xy_plane[0] = project(Vector3(-0.5, 0.5, 0.0), camera);
  this->xy_plane[1] = project(Vector3( 0.5, 0.5, 0.0), camera);
  this->xy_plane[2] = project(Vector3(-0.5,-0.5, 0.0), camera);
  this->xy_plane[3] = project(Vector3( 0.5,-0.5, 0.0), camera);
}


bool Prop3D::xy_plane_visible()
{
  // Check the depth component of each xy_plane corner
  // Return false if either point intersects the camera plane and should be culled/ignored
  return (
    this->xy_plane[0].z > 0.0 && this->xy_plane[0].z < 1.0 &&
    this->xy_plane[1].z > 0.0 && this->xy_plane[1].z < 1.0 &&
    this->xy_plane[2].z > 0.0 && this->xy_plane[2].z < 1.0 &&
    this->xy_plane[3].z > 0.0 && this->xy_plane[3].z < 1.0
  );
}



bool Prop3D::mouse_intersects(Vector2 mouse, Vector2 display)
{
  // Return true if mouse is within xy_plane
  // Determine this by calculating the dot product of the four polygons described by
  // each line of the quad and the mouse position;
  // The sign of the cross product z component indicates if the point lies inside or outside

  // We need 3-component vectors to calculate cross products
  Vector3 m3 = Vector3(mouse.x, mouse.y, 0.0);
  Vector3 p1 = Vector3(this->xy_plane[0].x, this->xy_plane[0].y, 0.0);
  Vector3 p2 = Vector3(this->xy_plane[1].x, this->xy_plane[1].y, 0.0);
  Vector3 p3 = Vector3(this->xy_plane[2].x, this->xy_plane[2].y, 0.0);
  Vector3 p4 = Vector3(this->xy_plane[3].x, this->xy_plane[3].y, 0.0);

  if ((m3 - p1).cross(p1 - p2).z < 0) return false;
  if ((m3 - p2).cross(p2 - p4).z < 0) return false;
  if ((m3 - p4).cross(p4 - p3).z < 0) return false;
  if ((m3 - p3).cross(p3 - p1).z < 0) return false;

  //std::cout << "Prop3D::mouse_intersects() returning TRUE" << std::endl;
  return true;
}



Vector2 Prop3D::relative_mouse_pos(Vector2 mouse, Vector2 display)
{
  // Return the mouse position relative to the xy_plane
  // Note: The resulting vector is scaled to 1;
  // Upper left corner = 0,0
  // Lower right corner = 1,1

  Vector2 v1 = mouse - xy_plane[0].xy();
  Vector2 v2 = mouse - xy_plane[1].xy();
  Vector2 v3 = mouse - xy_plane[2].xy();
  Vector2 v4 = mouse - xy_plane[3].xy();




  return mouse;
}


Obj3D* Prop3D::Object()
{
  return this->object;
}


void Prop3D::setObject(Obj3D* object)
{
  this->object = object;
}

void Prop3D::setScale(float scale)
{
  this->scale = Vector3(scale, scale, scale);
  this->need_recalc = true;
}

void Prop3D::setScale(Vector3 scale)
{
  this->scale = scale;
  this->need_recalc = true;
}

void Prop3D::setPosition(Vector3 position)
{
  this->pos = position;
  this->need_recalc = true;
}

void Prop3D::setDirection(Vector3 direction)
{
  this->dir = direction;
  this->need_recalc = true;
}

void Prop3D::setTexture(GLuint texture)
{
  this->texture = texture;
}

void Prop3D::setShader(ShaderProgram* shader)
{
  this->shader = shader;
  this->object->set_shader_v(this->shader->vertex_v);
  this->object->set_shader_vt(this->shader->vertex_vt);
  this->object->set_shader_vn(this->shader->vertex_vn);
}


