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
  this->material = nullptr;
  this->default_material.setFilename("");
  this->default_material.setName("default");
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
  std::cout << "Prop3D" << this << "::render() mesh=" << this->mesh << std::endl;
#endif

  this->mesh->bounds_enabled = this->bounds_enabled;
  if (this->texture_set) this->mesh->setTexture(this->texture);

  this->mesh->render(
    camera->getPerspectiveMatrix(),
    camera->getViewMatrix(),
    this->getMatrix(),
    (this->material == nullptr ? &this->default_material : this->material), // Mesh can override this
    this->shader // Mesh can override this
  );

  recalculate_xy_plane(camera);
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


void Prop3D::setTexture(Texture* texture)
{
  setTexture(texture->id());
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

  return point_object.xy;
}








