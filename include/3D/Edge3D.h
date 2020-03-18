#ifndef EDGE3D_H
#define EDGE3D_H

#include "Vectors.h"

class Edge3D
{
  public:
    Edge3D(Vector3 v1, Vector3 v2);
    Edge3D(Vector4 v1, Vector4 v2);
    ~Edge3D();

    Vector4 v1;
    Vector4 v2;

    bool operator==(const Edge3D& rhs)
    {
      return (this->v1 == rhs.v2 && this->v2 == rhs.v1) || (this->v1 == rhs.v1 && this->v2 == rhs.v2);
    }

  protected:

  private:
};



#endif // EDGE3D_H
