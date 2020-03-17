#ifndef SHADOWVOLUME3D_H
#define SHADOWVOLUME3D_H

#include <vector>

#include "3D/Light3D.h"
#include "3D/Mesh3D.h"
#include "Matrices.h"

class ShadowVolume3D : public Mesh3D
{
  public:
    ShadowVolume3D(Light3D* light, Mesh3D* mesh, Matrix4 model);
    ~ShadowVolume3D();

    void render(Matrix4 proj, Matrix4 view, Matrix4 model, Material* material, ShaderProgram* shader) {}
    void render(Matrix4 proj, Matrix4 view, Matrix4 model, GLenum face, ShaderProgram* shader);

  protected:

  private:
    void calc_dot_products_positional();
    void calc_dot_products_directional();

    void find_silouette_edges();

    std::vector<GLfloat> v;

    Light3D* light;
    Mesh3D* mesh;
    Matrix4 inverse_model;

};

#endif // SHADOWVOLUME3D_H
