#ifndef PROP3D_H
#define PROP3D_H

#include "Obj3D.h"
#include "Matrices.h"

/*

  A Prop3D object represents a unique instance of an Obj3D mesh in a Scene3D,
  with distinct position, orientation, texture and other attributes

*/


class Prop3D
{
  public:
    Prop3D();
    ~Prop3D();

    Obj3D* Object();
    void setObject(Obj3D* object);
    void setPosition(Vector3 position);

  protected:

  private:
    Obj3D* object;
    Matrix4 mat;
    Vector3 pos;
    bool need_recalc;
};

#endif // PROP3D_H
