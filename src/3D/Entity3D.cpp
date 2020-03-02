#include "Entity3D.h"

#include <iostream>

//#define DEBUG_TRACE_ENTITY

#define PI 3.1415
#define RAD2DEG 180.0 / PI

Entity3D::Entity3D()
{
  //ctor
  this->matrix = Matrix4();
  this->rotation_matrix = Matrix4();
  this->position_matrix = Matrix4();
  this->need_recalc = true;

  this->pitch = 0.0;
  this->yaw = 0.0;
  this->roll = 0.0;

  this->position = Vector3(0.0, 0.0, 0.0);
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << " created" << std::endl;
#endif
}

Entity3D::~Entity3D()
{
  //dtor
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << " destroyed" << std::endl;
#endif
}


Matrix4 Entity3D::getMatrix()
{
  //std::cout << "Entity3D::getMatrix()" << std::endl;
  if (this->need_recalc == true) this->recalculate_matrix();
  return this->matrix;
}

Matrix4 Entity3D::getRotationMatrix()
{
  if (this->need_recalc == false) return this->rotation_matrix;
  this->rotation_matrix = Matrix4()
    .rotate(this->pitch, Vector3(1.0f, 0.0f, 0.0f))
    .rotate(this->yaw, Vector3(0.0f, 1.0f, 0.0f))
    .rotate(this->roll, Vector3(0.0f, 0.0f, 1.0f));
  return this->rotation_matrix;
}

Matrix4 Entity3D::getPositionMatrix()
{
  if (this->need_recalc == false) return this->position_matrix;
  this->position_matrix = Matrix4().translate(Vector3(this->position.x, this->position.y, this->position.z));
  return this->position_matrix;
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






// Rotation ==================

void Entity3D::setDirection(float pitch, float yaw, float roll)
{
  this->pitch = pitch;
  this->yaw = yaw;
  this->roll = roll;
  this->need_recalc = true;
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << "::setDirection() pitch=" << pitch << " yaw=" << yaw << " roll=" << roll << std::endl;
#endif
}

void Entity3D::setDirection(Vector3 direction)
{
  this->pitch = asin(-direction.y) * RAD2DEG;
  this->yaw = atan2(direction.x, direction.z) * RAD2DEG;
  this->roll = 0;
  this->need_recalc = true;
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << "::setDirection() direction=" << direction << " pitch=" << pitch << " yaw=" << yaw << std::endl;
#endif
}

void Entity3D::setTargetDirection(Vector3 target)
{
  Vector3 direction = target - this->position;
  this->pitch = asin(-direction.y) * RAD2DEG;
  this->yaw = atan2(direction.x, direction.z) * RAD2DEG;
  this->roll = 0;
  this->need_recalc = true;
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << "::setTargetDirection() position=" << this->position << " target=" << target << " direction=" << direction << " pitch=" << pitch << " yaw=" << yaw << std::endl;
#endif
}

Vector3 Entity3D::getDirection()
{
  return Vector3(0.0, 0.0, -1.0) * getRotationMatrix(); // Objects face the camera by default
}

void Entity3D::setPitch(float degrees)
{
  this->pitch = degrees;
  // Clamp to sane values
  if (this->pitch > 90.0) this->pitch = 90;
  if (this->pitch < -90.0) this->pitch = -90;
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << "::setPitch() degrees=" << degrees << " pitch=" << pitch << std::endl;
#endif
  this->need_recalc = true;
}

void Entity3D::addPitch(float degrees)
{
  this->pitch += degrees;
  // Clamp to sane values
  if (this->pitch > 90.0) this->pitch = 90;
  if (this->pitch < -90.0) this->pitch = -90;
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << "::addPitch() degrees=" << degrees << " pitch=" << pitch << std::endl;
#endif
  this->need_recalc = true;
}

float Entity3D::getPitch()
{
  return this->pitch;
}

void Entity3D::setYaw(float degrees)
{
  this->yaw = degrees;
  // Clamp to sane values
  this->yaw = wrap(this->yaw, -360.0, 360.0);
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << "::setYaw() degrees=" << degrees << " yaw=" << yaw << std::endl;
#endif
  this->need_recalc = true;
}

void Entity3D::addYaw(float degrees)
{
  this->yaw += degrees;
  // Clamp to sane values
  this->yaw = wrap(this->yaw, -360.0, 360.0);
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << "::addYaw() degrees=" << degrees << " yaw=" << yaw << std::endl;
#endif
  this->need_recalc = true;
}

float Entity3D::getYaw()
{
  return this->yaw;
}

void Entity3D::setRoll(float degrees)
{
  this->roll = degrees;
  // Clamp to sane values
  this->roll = wrap(this->roll, -360.0, 360.0);
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << "::setRoll() degrees=" << degrees << " roll=" << roll << std::endl;
#endif
  this->need_recalc = true;
}

void Entity3D::addRoll(float degrees)
{
  this->roll += degrees;
  // Clamp to sane values
  this->roll = wrap(this->roll, -360.0, 360.0);
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << "::addRoll() degrees=" << degrees << " roll=" << roll << std::endl;
#endif
  this->need_recalc = true;
}

float Entity3D::getRoll()
{
  return this->roll;
}


// Position ==================

void Entity3D::setPosition(Vector3 position)
{
  this->position = position;
  this->need_recalc = true;
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << "::setPosition() vector=" << position << std::endl;
#endif
}

Vector3 Entity3D::getPosition()
{
  return this->position;
}


// Movement relative to direction ==================

void Entity3D::strafeLeft(float distance)
{
  Vector4 motion = Vector4(-distance, 0, 0, 0) * getRotationMatrix();
  this->position += motion.xyz();
  this->need_recalc = true;
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << "::strafeLeft() position=" << this->position << std::endl;
#endif
}

void Entity3D::strafeRight(float distance)
{
  Vector4 motion = Vector4(distance, 0, 0, 0) * getRotationMatrix();
  this->position += motion.xyz();
  this->need_recalc = true;
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << "::strafeRight() position=" << this->position << std::endl;
#endif
}

void Entity3D::strafeUp(float distance)
{
  Vector4 motion = Vector4(0, distance, 0, 0) * getRotationMatrix();
  this->position += motion.xyz();
  this->need_recalc = true;
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << "::strafeUp() position=" << this->position << std::endl;
#endif
}

void Entity3D::strafeDown(float distance)
{
  Vector4 motion = Vector4(0, -distance, 0, 0) * getRotationMatrix();
  this->position += motion.xyz();
  this->need_recalc = true;
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << "::strafeDown() position=" << this->position << std::endl;
#endif
}

void Entity3D::strafeForward(float distance)
{
  Vector4 motion = Vector4(0, 0, -distance, 0) * getRotationMatrix();
  this->position += motion.xyz();
  this->need_recalc = true;
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << "::strafeForward() position=" << this->position << std::endl;
#endif
}

void Entity3D::strafeBackward(float distance)
{
  Vector4 motion = Vector4(0, 0, distance, 0) * getRotationMatrix();
  this->position += motion.xyz();
  this->need_recalc = true;
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << "::strafeBackward() position=" << this->position << std::endl;
#endif
}


// Absolute movement ==================

void Entity3D::moveEast(float distance)
{
  this->position.x += distance;
  this->need_recalc = true;
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << "::moveEast() position=" << this->position << std::endl;
#endif
}

void Entity3D::moveWest(float distance)
{
  this->position.x -= distance;
  this->need_recalc = true;
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << "::moveWest() position=" << this->position << std::endl;
#endif
}

void Entity3D::moveUp(float distance)
{
  this->position.y += distance;
  this->need_recalc = true;
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << "::moveUp() position=" << this->position << std::endl;
#endif
}

void Entity3D::moveDown(float distance)
{
  this->position.y -= distance;
  this->need_recalc = true;
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << "::moveDown() position=" << this->position << std::endl;
#endif
}

void Entity3D::moveNorth(float distance)
{
  this->position.z += distance;
  this->need_recalc = true;
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << "::moveNorth() position=" << this->position << std::endl;
#endif
}

void Entity3D::moveSouth(float distance)
{
  this->position.z -= distance;
  this->need_recalc = true;
#ifdef DEBUG_TRACE_ENTITY
  std::cout << "Entity3D" << this << "::moveSouth() position=" << this->position << std::endl;
#endif
}


