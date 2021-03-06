#ifndef SUBOBJECT3D_H
#define SUBOBJECT3D_H

#include "3D/Box3D.h"
#include "3D/Light3D.h"
#include "3D/Mesh3D.h"
#include "ShaderProgram.h"

class SubObject3D : public Mesh3D
{
  public:
    SubObject3D();
    ~SubObject3D();

    void renderAmbient(Matrix4 proj, Matrix4 view, Matrix4 model, Material* material, ShaderProgram* shader);
    void renderLight(Matrix4 proj, Matrix4 view, Matrix4 model, Material* material, ShaderProgram* shader, Light3D* light);

    void generateShadowVolume(Light3D* light, Matrix4 model);
    void renderShadowVolume(Matrix4 proj, Matrix4 view, Matrix4 model, ShaderProgram* shader);
    void destroyShadowVolume();


  protected:

  private:
};

#endif // SUBOBJECT3D_H
