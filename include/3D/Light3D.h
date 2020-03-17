#ifndef LIGHT3D_H
#define LIGHT3D_H

#include "3D/Entity3D.h"
#include "Vectors.h"

class Light3D : public Entity3D
{
  public:
    Light3D(Vector3 color);
    ~Light3D();

    void enable();
    void disable();
    void setColor(Vector3 color);
    Vector3 getColor();
    void setPosition(Vector3 position);
    void setDirection(Vector3 direction);
    Vector3 getDirection();

    bool isDirectional();
    bool isPositional();
    bool isEnabled();

  protected:

  private:
    void recalculate_matrix();
    bool enabled = false;
    bool has_position = false;
    bool has_direction = false;
    Vector3 color;
    Vector3 direction;
};

std::ostream& operator <<(std::ostream& stream, Light3D* light);

#endif // LIGHT3D_H
