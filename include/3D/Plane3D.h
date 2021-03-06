#ifndef PLANE3D_H
#define PLANE3D_H

#include <3D/Light3D.h>
#include <3D/Mesh3D.h>


class Plane3D : public Mesh3D
{
  public:
    Plane3D();
    ~Plane3D();

    void renderAmbient(Matrix4 proj, Matrix4 view, Matrix4 model, Material* material, ShaderProgram* shader);
    void renderLight(Matrix4 proj, Matrix4 view, Matrix4 model, Material* material, ShaderProgram* shader, Light3D* light);

  protected:

  private:
    bool finalized;
    void finalize();
};

#endif // PLANE3D_H
