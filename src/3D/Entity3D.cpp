#include "Entity3D.h"

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
}

Entity3D::~Entity3D()
{
  //dtor
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
}

void Entity3D::setDirection(Vector3 direction)
{
  this->pitch = asin(-direction.y);
  this->yaw = atan2(direction.x, direction.z);
  this->roll = 0;
  this->need_recalc = true;
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
  //std::cout << "Entity3D::setPitch() degrees=" << degrees << " pitch=" << pitch << std::endl;
  this->need_recalc = true;
}

void Entity3D::addPitch(float degrees)
{
  this->pitch += degrees;
  // Clamp to sane values
  if (this->pitch > 90.0) this->pitch = 90;
  if (this->pitch < -90.0) this->pitch = -90;
  //std::cout << "Entity3D::addPitch() degrees=" << degrees << " pitch=" << pitch << std::endl;
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
  //std::cout << "Entity3D::setYaw() degrees=" << degrees << " yaw=" << yaw << std::endl;
  this->need_recalc = true;
}

void Entity3D::addYaw(float degrees)
{
  this->yaw += degrees;
  // Clamp to sane values
  this->yaw = wrap(this->yaw, -360.0, 360.0);
  //std::cout << "Entity3D::addYaw() degrees=" << degrees << " yaw=" << yaw << std::endl;
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
  //std::cout << "Entity3D::setRoll() degrees=" << degrees << " roll=" << roll << std::endl;
  this->need_recalc = true;
}

void Entity3D::addRoll(float degrees)
{
  this->roll += degrees;
  // Clamp to sane values
  this->roll = wrap(this->roll, -360.0, 360.0);
  //std::cout << "Entity3D::addRoll() degrees=" << degrees << " roll=" << roll << std::endl;
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
}

Vector3 Entity3D::getPosition()
{
  return this->position;
}


// Movement relative to direction ==================

void Entity3D::strafeLeft(float distance)
{
  Vector4 motion = Vector4(distance, 0, 0, 0) * getRotationMatrix();
  this->position += motion.xyz();
  this->need_recalc = true;
}

void Entity3D::strafeRight(float distance)
{
  Vector4 motion = Vector4(-distance, 0, 0, 0) * getRotationMatrix();
  this->position += motion.xyz();
  this->need_recalc = true;
}

void Entity3D::strafeUp(float distance)
{
  Vector4 motion = Vector4(0, -distance, 0, 0) * getRotationMatrix();
  this->position += motion.xyz();
  this->need_recalc = true;
}

void Entity3D::strafeDown(float distance)
{
  Vector4 motion = Vector4(0, distance, 0, 0) * getRotationMatrix();
  this->position += motion.xyz();
  this->need_recalc = true;
}

void Entity3D::strafeForward(float distance)
{
  Vector4 motion = Vector4(0, 0, distance, 0) * getRotationMatrix();
  this->position += motion.xyz();
  this->need_recalc = true;
}

void Entity3D::strafeBackward(float distance)
{
  Vector4 motion = Vector4(0, 0, -distance, 0) * getRotationMatrix();
  this->position += motion.xyz();
  this->need_recalc = true;
}


// Absolute movement ==================

void Entity3D::moveEast(float distance)
{
  this->position.x += distance;
  this->need_recalc = true;
}

void Entity3D::moveWest(float distance)
{
  this->position.x -= distance;
  this->need_recalc = true;
}

void Entity3D::moveUp(float distance)
{
  this->position.y -= distance;
  this->need_recalc = true;
}

void Entity3D::moveDown(float distance)
{
  this->position.y += distance;
  this->need_recalc = true;
}

void Entity3D::moveNorth(float distance)
{
  this->position.z += distance;
  this->need_recalc = true;
}

void Entity3D::moveSouth(float distance)
{
  this->position.z -= distance;
  this->need_recalc = true;
}


