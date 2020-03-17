#ifndef SHADOWVOLUME3D_H
#define SHADOWVOLUME3D_H

#include "3D/Light3D.h"
#include "3D/Mesh3D.h"


class ShadowVolume3D : public Mesh3D
{
  public:
    ShadowVolume3D(Light3D* light, Mesh3D* mesh);
    ~ShadowVolume3D();

    void render(Matrix4 proj, Matrix4 view, Matrix4 model, Material* material, ShaderProgram* shader);

  protected:

  private:
    void calc_dot_products_positional();
    void calc_dot_products_directional();


    Light3D* light;
    Mesh3D* mesh;
};

#endif // SHADOWVOLUME3D_H
