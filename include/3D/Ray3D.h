#ifndef RAY3D_H
#define RAY3D_H

#include "3D/Mesh3D.h"
#include "3D/Camera3D.h"

class Ray3D : public Mesh3D
{
  public:
    Ray3D();
    ~Ray3D();

    void render(ShaderProgram* shader);

  protected:

  private:
    bool finalized;
    void finalize();

};

#endif // RAY3D_H
