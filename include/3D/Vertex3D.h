#ifndef VERTEX3D_H
#define VERTEX3D_H

#include <vector>

#include "Vectors.h"

class Vertex3D
{
  public:
    Vertex3D(Vector3 position);
    ~Vertex3D();

    void connectFace(uint32_t face_id);

    Vector3 v;
    Vector2 vt;
    Vector3 vn;
    uint32_t index;

    std::vector<uint32_t> faces;

  protected:
  private:
};

#endif

