#include "Camera3D.h"

Camera3D::Camera3D()
{
  //ctor
  this->fov = 60;
  this->aspect = 640.0 / 480.0; // 1.333
  this->pitch = 0.0;
  this->yaw = 0.0;
  this->pos = Vector3(0.0, 0.0, -1.0);
  this->need_recalc = true;
  //std::cout << "Camera3D() created" << std::endl;
}

Camera3D::~Camera3D()
{
  //dtor
  //std::cout << "Camera3D() destroyed" << std::endl;
}


// Wrap number so it stays within lower-upper boundary (e.g. -360..360 degrees)
// Note: boundaries must be INTEGER values
float wrap( float target, int lower, int upper){
  int dif = upper - lower;

  if(target > lower)
    return lower + (int)(target - lower) % dif;

  target = lower + (lower - target);
  float tmp = lower + (int)(target - lower) % dif;
  return upper - tmp;
}

Matrix4 Camera3D::matrix()
{
  if (this->need_recalc == true) this->recalculate();
  return this->mat;
}

void Camera3D::recalculate()
{
  //std::cout << "Camera3D::recalculate() begin" << std::endl;
  this->mat = Matrix4().Perspective(this->fov, this->aspect, 0.1f, 100.0f);
  this->mat *= Matrix4().rotate(this->pitch, Vector3(1.0f, 0.0f, 0.0f));
  this->mat *= Matrix4().rotate(this->yaw, Vector3(0.0f, 1.0f, 0.0f));
  //this->mat *= Matrix4().rotate(180, Vector3(0.0f, 0.0f, 1.0f)); // Why is the scene up-side-down??
  this->mat *= Matrix4().translate(Vector3( this->pos.x, this->pos.y, this->pos.z));
  //std::cout << "Camera3D::recalculate() done" << std::endl;
  this->need_recalc = false;
}

void Camera3D::setPosition(Vector3 position)
{
  this->pos = position;
  this->need_recalc = true;
}

void Camera3D::setDimensions(int width, int height)
{
  this->width = width;
  this->height = height;
  this->aspect = (float) width / (float) height;
  //std::cout << "Camera3D::setDimensions() width=" << width << " height=" << height << " ratio=" << this->aspect << std::endl;
  this->need_recalc = true;
}

void Camera3D::setAspect(float aspect_ratio)
{
  this->aspect = aspect_ratio;
  // Clamp to sane values
  if (this->aspect > 10.0) this->aspect = 10.0;
  if (this->aspect < 0.01) this->aspect = 0.01;
  //std::cout << "Camera3D::setAspect() ratio=" << aspect_ratio << std::endl;
  this->need_recalc = true;
}

int Camera3D::getWidth()
{
  return this->width;
}

int Camera3D::getHeight()
{
  return this->height;
}

float Camera3D::getAspect()
{
  return this->aspect;
}


void Camera3D::setFOV(float degrees)
{
  this->fov = degrees;
  // Clamp to sane values
  if (this->fov > 140) this->fov = 140;
  if (this->fov < 20) this->fov = 20;
  //std::cout << "Camera3D::setFOV() degrees=" << degrees << " fov=" << fov << std::endl;
  this->need_recalc = true;
}

void Camera3D::addFOV(float degrees)
{
  this->fov += degrees;
  // Clamp to sane values
  if (this->fov > 140) this->fov = 140;
  if (this->fov < 20) this->fov = 20;
  //std::cout << "Camera3D::addFOV() degrees=" << degrees << " fov=" << fov << std::endl;
  this->need_recalc = true;
}

float Camera3D::getFOV()
{
  return this->fov;
}

void Camera3D::setPitch(float degrees)
{
  this->pitch = degrees;
  // Clamp to sane values
  if (this->pitch > 90.0) this->pitch = 90;
  if (this->pitch < -90.0) this->pitch = -90;
  //std::cout << "Camera3D::setPitch() degrees=" << degrees << " pitch=" << pitch << std::endl;
  this->need_recalc = true;
}

void Camera3D::addPitch(float degrees)
{
  this->pitch += degrees;
  // Clamp to sane values
  if (this->pitch > 90.0) this->pitch = 90;
  if (this->pitch < -90.0) this->pitch = -90;
  //std::cout << "Camera3D::addPitch() degrees=" << degrees << " pitch=" << pitch << std::endl;
  this->need_recalc = true;
}

float Camera3D::getPitch()
{
  return this->pitch;
}

void Camera3D::setYaw(float degrees)
{
  this->yaw = degrees;
  // Clamp to sane values
  this->yaw = wrap(this->yaw, -360.0, 360.0);
  //std::cout << "Camera3D::setYaw() degrees=" << degrees << " yaw=" << yaw << std::endl;
  this->need_recalc = true;
}

void Camera3D::addYaw(float degrees)
{
  this->yaw += degrees;
  // Clamp to sane values
  this->yaw = wrap(this->yaw, -360.0, 360.0);
  //std::cout << "Camera3D::addYaw() degrees=" << degrees << " yaw=" << yaw << std::endl;
  this->need_recalc = true;
}

float Camera3D::getYaw()
{
  return this->yaw;
}

