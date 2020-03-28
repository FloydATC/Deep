#include "3D/Camera3D.h"

#include <iostream>

//#define DEBUG_TRACE_CAMERA

Camera3D::Camera3D()
{
  //ctor
  this->fov = 60;
  this->aspect = 640.0f / 480.0f; // 1.333
#ifdef DEBUG_TRACE_CAMERA
  std::cout << "Camera3D" << this << " created" << std::endl;
#endif
}

Camera3D::~Camera3D()
{
  //dtor
#ifdef DEBUG_TRACE_CAMERA
  std::cout << "Camera3D" << this << " destroyed" << std::endl;
#endif
}



Matrix4 Camera3D::getPerspectiveMatrix()
{
  if (this->need_recalc == false) return this->perspective_matrix;
  this->perspective_matrix = Matrix4().Perspective(this->fov, this->aspect, this->clip_near, this->clip_far);
  return this->perspective_matrix;
}

Matrix4 Camera3D::getRotationMatrix()
{
  // Camera rotation must be applied in reverse order
  if (this->need_recalc == false) return this->rotation_matrix;
  this->rotation_matrix = Matrix4()
    .rotate(-this->roll, Vector3(0.0f, 0.0f, 1.0f))
    .rotate(-this->yaw, Vector3(0.0f, 1.0f, 0.0f))
    .rotate(-this->pitch, Vector3(1.0f, 0.0f, 0.0f));
  return this->rotation_matrix;
}

Matrix4 Camera3D::getPositionMatrix()
{
  // Camera movement must be applied in opposite direction
  if (this->need_recalc == false) return this->position_matrix;
  this->position_matrix = Matrix4().translate(Vector3(-this->position.x, -this->position.y, -this->position.z));
  return this->position_matrix;
}


Matrix4 Camera3D::getViewMatrix()
{
  if (this->need_recalc == false) return this->view_matrix;
  this->view_matrix = getRotationMatrix() * getPositionMatrix();
  return this->view_matrix;
}

void Camera3D::recalculate_matrix()
{
#ifdef DEBUG_TRACE_CAMERA
  std::cout << "Camera3D" << this << "::recalculate_matrix() begin" << std::endl;
#endif
  this->matrix  = getPerspectiveMatrix() * getViewMatrix();
  //std::cout << "Camera3D::recalculate_matrix() done" << std::endl;
  this->need_recalc = false;
}



void Camera3D::setDimensions(int width, int height)
{
  this->width = width;
  this->height = height;
  this->aspect = (float) width / (float) height;
#ifdef DEBUG_TRACE_CAMERA
  std::cout << "Camera3D" << this << "::setDimensions() width=" << width << " height=" << height << " ratio=" << this->aspect << std::endl;
#endif
  this->need_recalc = true;
}

void Camera3D::setAspect(float aspect_ratio)
{
  this->aspect = aspect_ratio;
  // Clamp to sane values
  if (this->aspect > 10.0) this->aspect = 10.0f;
  if (this->aspect < 0.01) this->aspect = 0.01f;
#ifdef DEBUG_TRACE_CAMERA
  std::cout << "Camera3D" << this << "::setAspect() ratio=" << aspect_ratio << std::endl;
#endif
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
#ifdef DEBUG_TRACE_CAMERA
  std::cout << "Camera3D" << this << "::setFOV() degrees=" << degrees << " fov=" << fov << std::endl;
#endif
  this->need_recalc = true;
}

void Camera3D::addFOV(float degrees)
{
  this->fov += degrees;
  // Clamp to sane values
  if (this->fov > 140) this->fov = 140;
  if (this->fov < 20) this->fov = 20;
#ifdef DEBUG_TRACE_CAMERA
  std::cout << "Camera3D" << this << "::addFOV() degrees=" << degrees << " fov=" << fov << std::endl;
#endif
  this->need_recalc = true;
}

float Camera3D::getFOV()
{
  return this->fov;
}

float Camera3D::getNear()
{
  return this->clip_near;
}

float Camera3D::getFar()
{
  return this->clip_far;
}

Vector3 Camera3D::getDirection()
{
  return Vector3(0.0, 0.0, 1.0) * getRotationMatrix(); // Camera points forward by default
}



