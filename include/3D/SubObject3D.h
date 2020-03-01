#ifndef SUBOBJECT3D_H
#define SUBOBJECT3D_H

#include "3D/Mesh3D.h"
#include "3D/Box3D.h"
#include "ShaderProgram.h"

class SubObject3D : public Mesh3D
{
  public:
    SubObject3D();
    ~SubObject3D();

    void render(ShaderProgram* shader);

  protected:

  private:
};

#endif // SUBOBJECT3D_H
