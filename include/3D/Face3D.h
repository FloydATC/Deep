#ifndef FACE3D_H
#define FACE3D_H

#include <vector>

#include "Vectors.h"
#include "3D/Edge3D.h"
#include "3D/Vertex3D.h"


class Face3D
{
  public:
    Face3D(Vertex3D* v1, Vertex3D* v2, Vertex3D* v3);
    ~Face3D();

    Edge3D getEdge(uint8_t index);
    void setAdjacentFace(uint8_t edge_no, uint32_t face_id);

    std::vector<Vertex3D*> vertices;

    std::vector<int> adjacent; // Per edge, Face3D::addAdjacentFace()

    Vector3 normal;
    Vector3 center;
    float dot;

  protected:

  private:
    Vector3 calc_normal();
    Vector3 calc_center();
};

#endif // FACE3D_H
