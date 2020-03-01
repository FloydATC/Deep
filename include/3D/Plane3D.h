#ifndef PLANE3D_H
#define PLANE3D_H

#include <3D/Mesh3D.h>


class Plane3D : public Mesh3D
{
  public:
    Plane3D();
    ~Plane3D();

    void render(ShaderProgram* shader);

  protected:

  private:
    bool finalized;
    void finalize();
};

#endif // PLANE3D_H
