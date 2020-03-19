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

    void renderAmbient(Matrix4 proj, Matrix4 view, Matrix4 model, Material* material, ShaderProgram* shader) {}
    void renderLight(Matrix4 proj, Matrix4 view, Matrix4 model, Material* material, ShaderProgram* shader, Light3D* light) {}

    void renderShadow(Matrix4 proj, Matrix4 view, Matrix4 model, ShaderProgram* shader);

  protected:

  private:
    void compute_positional();
    void compute_directional();
    void set_v(float* v, int num_vertices);
    void initialize(ShaderProgram* shader);


    std::vector<GLfloat> v;

    Light3D* light;
    Mesh3D* mesh;
    Matrix4 inverse_model;

};

#endif // SHADOWVOLUME3D_H
