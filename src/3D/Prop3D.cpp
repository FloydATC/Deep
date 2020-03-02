#include "3D/Prop3D.h"

#include "Matrices.h"
#include "3D/Math3D.h"

#include "Scene3D.h" // debugging

//#define DEBUG_TRACE_PROP

Prop3D::Prop3D()
{
  //ctor
  this->mesh = nullptr;
  this->shader = nullptr;
  this->scale = Vector3(1.0, 1.0, 1.0);
  this->scale_matrix = Matrix4();
  this->xy_plane.resize(4); // 4 x Vector2
  this->texture = 0;
  this->texture_set = false;
  this->bounds_enabled = false;
#ifdef DEBUG_TRACE_PROP
  std::cout << "Prop3D" << this << " created" << std::endl;
#endif
}

Prop3D::~Prop3D()
{
  //dtor
#ifdef DEBUG_TRACE_PROP
  std::cout << "Prop3D" << this << " destroyed" << std::endl;
#endif
}


void Prop3D::render(Camera3D* camera) {
  if (this->need_recalc) this->recalculate_matrix();
#ifdef DEBUG_TRACE_PROP
  std::cout << "Prop3D" << this << "::render() called for object '" << this->mesh->getName() << "'" << std::endl;
#endif

  if (this->shader == nullptr) {
    std::cerr << "Prop3D" << this  << "::render() no shader!" << std::endl;
    return;
  }

  glUseProgram(this->shader->id());

  // Textures can be attached to underlying Mesh3D objects or to individual Prop3D objects
  if (this->texture_set) {
    //std::cout << "Prop3D" << this << "::render() bind texture " << this->texture << std::endl;
    shader->setTextureFlag(true);
    glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
    glBindTexture(GL_TEXTURE_2D, this->texture);
  } else {
    //std::cout << "Prop3D" << this << "::render() has no texture set" << std::endl;
    shader->setTextureFlag(false);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  this->shader->setCameraMatrix(camera->getMatrix());
  this->shader->setModelMatrix(this->matrix);
  //std::cout << "Prop3D" << this << "::render() model matrix:" << std::endl;
  //std::cout << this->matrix << std::endl;

  this->mesh->bounds_enabled = this->bounds_enabled;
  this->mesh->render(this->shader); // Render Mesh3D

  recalculate_xy_plane(camera);
  //std::cout << "Prop3D" << this << "::render() done" << std::endl;
}


Matrix4 Prop3D::getScaleMatrix()
{
  if (this->need_recalc == false) return this->scale_matrix;
  this->scale_matrix = Matrix4().scale(this->scale.x, this->scale.y, this->scale.z);
  return this->scale_matrix;
}


void Prop3D::recalculate_matrix()
{
#ifdef DEBUG_TRACE_PROP
  std::cout << "Prop3D" << this << "::recalculate_matrix()" << std::endl;
#endif
  this->matrix  = getPositionMatrix(); // Matrix4().translate(this->position); // Relative position
  this->matrix *= getRotationMatrix();
  this->matrix *= getScaleMatrix();
  this->need_recalc = false;
}



Mesh3D* Prop3D::getMesh()
{
  return this->mesh;
}


void Prop3D::setMesh(Mesh3D* mesh)
{
#ifdef DEBUG_TRACE_PROP
  std::cout << "Prop3D" << this << "::setMesh() mesh=" << mesh << " name=" << mesh->getName() << std::endl;
#endif
  this->mesh = mesh;
}


void Prop3D::setScale(float scale)
{
#ifdef DEBUG_TRACE_PROP
  std::cout << "Prop3D" << this << "::setScale() scale=" << scale << std::endl;
#endif
  this->scale = Vector3(scale, scale, scale);
  this->need_recalc = true;
}


void Prop3D::setScale(Vector3 scale)
{
#ifdef DEBUG_TRACE_PROP
  std::cout << "Prop3D" << this << "::setScale() scale=" << scale << std::endl;
#endif
  this->scale = scale;
  this->need_recalc = true;
}


void Prop3D::setShader(ShaderProgram* shader)
{
#ifdef DEBUG_TRACE_PROP
  std::cout << "Prop3D" << this << "::setShader() shader=" << shader << std::endl;
#endif
  this->shader = shader;
}


void Prop3D::setTexture(GLuint texture)
{
#ifdef DEBUG_TRACE_PROP
  std::cout << "Prop3D" << this << "::setTexture() texture=" << texture << " enabled" << std::endl;
#endif
  this->texture = texture;
  this->texture_set = true;
}

void Prop3D::showBounds()
{
#ifdef DEBUG_TRACE_PROP
  std::cout << "Prop3D" << this << "::showBounds()" << std::endl;
#endif
  this->bounds_enabled = true;
}

void Prop3D::hideBounds()
{
#ifdef DEBUG_TRACE_PROP
  std::cout << "Prop3D" << this << "::hideBounds()" << std::endl;
#endif
  this->bounds_enabled = false;
}






////// Experimental code follows -- beware of dragons //////


/*
Vector3 Prop3D::project(Vector3 v3, Camera3D* camera)
{
  // This function is used to project (transform) each of the four xy_plane corners
  // from world coordinates onto the screen. We will use this 2-dimensional plane
  // to pinpoint mouse events relative to the 3D object.

  // We need a Vector4 for the projection since the matrices are 4x4 and use perspective
  Vector4 v4 =  camera->getMatrix() * this->matrix * Vector4(v3.x, v3.y, v3.z, 1.0);

  // Return 2D display coordinates + depth component
  int w2 = camera->getWidth() / 2;
  int h2 = camera->getHeight() / 2;
  return Vector3(w2+(v4.x*w2/v4.w), h2+(v4.y*-h2/v4.w), v4.z/v4.w); // flip Y so 0,0 is upper left corner
}
*/

void Prop3D::recalculate_xy_plane(Camera3D* camera)
{
  // This function is called right after we render the prop in 3D,
  // as we then have both the camera and model matrix available.
  // The Obj3D size must be (roughly) 1.0 and it must be (roughly) centered at 0,0,0

  Vector2 display = camera->getDimensions();
  Matrix4 projection = camera->getPerspectiveMatrix();
  Matrix4 view = camera->getViewMatrix();

  // Calculate 4 corners of Obj3D XY plane
  // NOTE: The corners are listed in clockwise order!
  xy_plane[0] = projected_vector(Vector3(-0.5, 0.5, 0.0), display, projection, view, this->matrix);
  xy_plane[1] = projected_vector(Vector3( 0.5, 0.5, 0.0), display, projection, view, this->matrix);
  xy_plane[2] = projected_vector(Vector3( 0.5,-0.5, 0.0), display, projection, view, this->matrix);
  xy_plane[3] = projected_vector(Vector3(-0.5,-0.5, 0.0), display, projection, view, this->matrix);
}


bool Prop3D::xy_plane_visible()
{
  // Check the depth component of each xy_plane corner
  // Return false if either point intersects the camera plane and should be culled/ignored
  return (
    xy_plane[0].z > 0.0 && xy_plane[0].z < 1.0 &&
    xy_plane[1].z > 0.0 && xy_plane[1].z < 1.0 &&
    xy_plane[2].z > 0.0 && xy_plane[2].z < 1.0 &&
    xy_plane[3].z > 0.0 && xy_plane[3].z < 1.0
  );
}



bool Prop3D::mouse_intersects(Vector2 mouse)
{
  return point_inside_convex_polygon(mouse, this->xy_plane);
}


Vector2 Prop3D::relative_mouse_pos(Vector2 mouse, Camera3D* camera)
{
  //std::cout << "Prop3D" << this << "::relative_mouse_pos" << std::endl;
  // Return the mouse position relative to the xy_plane of this object
  // Upper left corner  = -0.5, 0.5
  // Center             =  0.0, 0.0
  // Lower right corner =  0.5,-0.5

  // Calculate the direction of an imaginary ray from the "eye" through the mouse pointer
  Vector3 ray_world = unprojected_vector(
    mouse,
    camera->getDimensions(),
    camera->getPerspectiveMatrix(),
    camera->getViewMatrix()
  );

  // Calculate where the ray hits the xy_plane of this object
  // Note: Checking against an imaginary plane fails to take into account
  // the curvature of the display so the end result is not perfect.
  // Reading from the depth buffer would be better, but getting that
  // information through all the transforms is a major challenge.
  // Also, the depth buffer's loss of precision at range could be a problem
  Vector3 point_world = ray_plane_intersect(
    camera->getPosition(),
    ray_world,
    this->getPosition(),
    this->getDirection()
  );

  // Finally, transform the coordinates to this object's local coordinate system
  // If we did everything right, Z should be very close to zero
  Matrix4 model_matrix = this->matrix;
  Vector3 point_object = model_matrix.invert() * point_world;
  //std::cout << "  point_object = " << point_object << std::endl;

  return point_object.xy();
}








