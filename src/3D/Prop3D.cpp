#include "3D/Prop3D.h"

#include "Matrices.h"

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




////// Experimental code follows -- beware of dragons //////



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


void Prop3D::recalculate_xy_plane(Camera3D* camera)
{
  // This function is called right after we render the prop in 3D,
  // as we then have both the camera and model matrix available.
  // The Obj3D size must be (roughly) 1.0 and it must be (roughly) centered at 0,0,0

  // Calculate 4 corners of Obj3D XY plane
  xy_plane[0] = project(Vector3(-0.5, 0.5, 0.0), camera);
  xy_plane[1] = project(Vector3( 0.5, 0.5, 0.0), camera);
  xy_plane[2] = project(Vector3(-0.5,-0.5, 0.0), camera);
  xy_plane[3] = project(Vector3( 0.5,-0.5, 0.0), camera);
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



bool Prop3D::mouse_intersects(Vector2 mouse, Vector2 display)
{
  // Return true if mouse is within xy_plane
  // Determine this by calculating the dot product of the four polygons described by
  // each line of the quad and the mouse position;
  // The sign of the cross product z component indicates if the point lies inside or outside

  // We need 3-component vectors to calculate cross products
  Vector3 m3 = Vector3(mouse.x, mouse.y, 0.0);
  Vector3 p0 = Vector3(xy_plane[0].x, xy_plane[0].y, 0.0);
  Vector3 p1 = Vector3(xy_plane[1].x, xy_plane[1].y, 0.0);
  Vector3 p2 = Vector3(xy_plane[2].x, xy_plane[2].y, 0.0);
  Vector3 p3 = Vector3(xy_plane[3].x, xy_plane[3].y, 0.0);

  // Check each edge
  if ((m3 - p0).cross(p0 - p1).z < 0) return false;
  if ((m3 - p1).cross(p1 - p3).z < 0) return false;
  if ((m3 - p3).cross(p3 - p2).z < 0) return false;
  if ((m3 - p2).cross(p2 - p0).z < 0) return false;

  //std::cout << "The mouse is inside the quadrilateral polygon" << std::endl;
  // ...but where? Tune in for the next exciting episode in relative_mouse_pos()

  return true;
}





Vector2 Prop3D::relative_mouse_pos(Vector2 mouse, Camera3D* camera, void* scene)
{                                                                // ^^^^^^^^^^^^^^ DEBUGGING
  std::cout << "Prop3D" << this << "::relative_mouse_pos" << std::endl;
  // Return the mouse position relative to the xy_plane of this object
  // Upper left corner = 0,0
  // Lower right corner = 1,1

  std::cout << "  object position = WS " << this->position << std::endl;
  std::cout << "  camera position = WS " << camera->getPosition() << std::endl;

  GLint viewport[4] = { 0, 0, camera->getWidth(), camera->getHeight() };
  GLdouble modelview[16];
  camera->getViewMatrixDoubleV(modelview);
  GLdouble projection[16];
  camera->getPerspectiveMatrixDoubleV(projection);

  // Read depth buffer at mouse coordinates
  GLint winX = mouse.x;
  GLint winY = viewport[3] - mouse.y;
  GLint winZ;
  glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
  std::cout << "  gluUnProject winX=" << winX << " winY=" << winY << " winZ=" << winZ << std::endl;

  GLdouble posX, posY, posZ;
  gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

  std::cout << "  gluUnProject posX=" << posX << " posY=" << posY << " posZ=" << posZ << std::endl;
  //((Scene3D*)scene)->getProp(5)->setPosition(mouse_worldspace.xyz()); // DEBUGGING

  Vector3 mouse_worldspace = Vector3(posX, posY, posZ);

  Prop3D* white_cube = ((Scene3D*)scene)->getProp(4);
  Prop3D* magenta_ray = ((Scene3D*)scene)->getProp(5);
  white_cube->setPosition(mouse_worldspace); // DEBUGGING


  // origin of camera in worldspace
  Vector3 ray_origin = Vector3(0,0,0); // camera->getPosition();
  magenta_ray->setPosition(ray_origin);


  // unit vector from p through mouse pos in worldspace
  Vector3 ray_world = Vector3(mouse_worldspace - ray_origin).normalize();

  magenta_ray->setDirection(this->position);





  return Vector2(rand(), rand()); // Might as well do this
}








