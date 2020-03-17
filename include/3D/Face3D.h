#ifndef FACE3D_H
#define FACE3D_H

#include <vector>

#include "Vectors.h"
#include "3D/Edge3D.h"

class Face3D
{
  public:
    Face3D(Vector3 v1, Vector3 v2, Vector3 v3);
    ~Face3D();

    Edge3D edge(unsigned int index);

    std::vector<Vector3> vertices;
    std::vector<int> adjacent;
    Vector3 normal;
    float dot;

  protected:

  private:
    Vector3 calc_normal();
};

#endif // FACE3D_H
