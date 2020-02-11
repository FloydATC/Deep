#include "Prop3D.h"

Prop3D::Prop3D()
{
  //ctor
  this->pos = Vector3(0.0, 0.0, 0.0);
  this->need_recalc = true;
}

Prop3D::~Prop3D()
{
  //dtor
}


Obj3D* Prop3D::Object()
{
  return this->object;
}


void Prop3D::setObject(Obj3D* object)
{
  this->object = object;
}


void Prop3D::setPosition(Vector3 position)
{
  this->pos = position;
  this->need_recalc = true;
}

