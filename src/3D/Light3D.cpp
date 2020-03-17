#include "Light3D.h"

Light3D::Light3D(Vector3 color)
{
  //ctor
  this->color = color;
  this->enabled = true;
}

Light3D::~Light3D()
{
  //dtor
}


void Light3D::enable()
{
  this->enabled = true;
}


void Light3D::disable()
{
  this->enabled = false;
}


void Light3D::setColor(Vector3 color)
{
  this->color = color;
}


Vector3 Light3D::getColor()
{
  return this->color;
}


void Light3D::setPosition(Vector3 position)
{
  this->position = position;
  this->has_position = true;
  this->has_direction = false;
}


void Light3D::setDirection(Vector3 direction)
{
  this->direction = direction;
  this->has_direction = true;
  this->has_position = false;
}


Vector3 Light3D::getDirection()
{
  return this->direction;
}


bool Light3D::isDirectional()
{
  return this->has_direction == true;
}


bool Light3D::isPositional()
{
  return this->has_position == true;
}


bool Light3D::isEnabled()
{
  return this->enabled && (this->has_position || this->has_direction);
}


void Light3D::recalculate_matrix()
{
  // Irrelevant for lights
}


std::ostream& operator <<(std::ostream& stream, Light3D* light)
{
  if (light == nullptr) {
    stream << "<no light>";
    return stream;
  } else {
    stream << "<Light3D color=" << light->getColor() << " ";
    if (light->isPositional()) stream << "positional=" << light->getPosition();
    if (light->isDirectional()) stream << "directional=" << light->getDirection();
    stream << " " << (light->isEnabled() ? "(enabled)" : "(disabled)") << " >";
    return stream;
  }
}
