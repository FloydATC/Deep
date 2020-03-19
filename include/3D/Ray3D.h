#ifndef RAY3D_H
#define RAY3D_H

#include "3D/Light3D.h"
#include "3D/Mesh3D.h"

class Ray3D : public Mesh3D
{
  public:
    Ray3D();
    ~Ray3D();

    void renderAmbient(Matrix4 proj, Matrix4 view, Matrix4 model, Material* material, ShaderProgram* shader);
    void renderLight(Matrix4 proj, Matrix4 view, Matrix4 model, Material* material, ShaderProgram* shader, Light3D* light) {}

  protected:

  private:
    bool finalized;
    void finalize();

};

#endif // RAY3D_H
