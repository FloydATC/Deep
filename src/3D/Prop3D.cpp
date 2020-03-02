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
  // Upper left corner  = -0.5, 0.5
  // Center             =  0.0, 0.0
  // Lower right corner =  0.5,-0.5

  // We need copies of the following matrices
  Matrix4 projection_matrix = camera->getPerspectiveMatrix();
  Matrix4 view_matrix = camera->getViewMatrix();
  Matrix4 model_matrix = this->matrix;

  // http://antongerdelan.net/opengl/raycasting.html
  float x = (2.0f * mouse.x) / camera->getWidth() - 1.0f;
  float y = 1.0f - (2.0f * mouse.y) / camera->getHeight();
  float z = 1.0f;
  Vector3 mouse_nds = Vector3(x, y, z); // normalized device space
  Vector4 mouse_clip = Vector4(mouse_nds.x, mouse_nds.y, -1.0, 1.0); // homogeneous clip space
  Vector4 mouse_eye = projection_matrix.invert() * mouse_clip;
  mouse_eye = Vector4(mouse_eye.x, mouse_eye.y, -1.0, 0.0); // eye space
  Vector3 ray_world = (view_matrix.invert() * mouse_eye).xyz();
  ray_world = ray_world.normalize(); // direction in world space
  std::cout << "  ray_world = " << ray_world << std::endl;

  // Visualize ray_world
  Prop3D* white_cube1 = ((Scene3D*)scene)->getProp(4);
  Prop3D* white_cube2 = ((Scene3D*)scene)->getProp(5);
  Prop3D* magenta_ray = ((Scene3D*)scene)->getProp(6);
  white_cube1->setPosition(camera->getPosition());
  white_cube1->setDirection(ray_world);



  Vector3 planeP = this->getPosition();
  Vector3 planeN = this->getDirection();
  Vector3 rayP = camera->getPosition();
  Vector3 rayD = ray_world;
  float epsilon = 0.00001;

  // https://stackoverflow.com/questions/23975555/how-to-do-ray-plane-intersection
  float denom = planeN.dot(rayD);
  std::cout << "  denom = " << denom << " abs(denom) = " << fabs(denom) << std::endl;
  if (fabs(denom) < epsilon) return Vector2(-1,-1); // No intersection, ray is paralell
  float t = (planeP - rayP).dot(planeN) / denom;
  Vector3 point_world = rayP + t * rayD;

  std::cout << "  point_world = " << point_world << std::endl;
  white_cube2->setPosition(point_world);
  white_cube2->setTargetDirection(camera->getPosition());

  Vector3 ray_origin = Vector3(0,0,1); // camera->getPosition();
  magenta_ray->setPosition(ray_origin);
  magenta_ray->setTargetDirection(white_cube2->getPosition());

  Vector3 point_object = model_matrix.invert() * point_world;
  std::cout << "  point_object = " << point_object << std::endl;


  return Vector2(rand(), rand()); // Might as well do this
}








