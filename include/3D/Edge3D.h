#ifndef EDGE3D_H
#define EDGE3D_H

#include "Vectors.h"

#define EPSILON 0.00001

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
      return (this->v1.equal(rhs.v2, EPSILON) && this->v2.equal(rhs.v1, EPSILON))
          || (this->v1.equal(rhs.v1, EPSILON) && this->v2.equal(rhs.v2, EPSILON));
    }

  protected:

  private:
};



#endif // EDGE3D_H
