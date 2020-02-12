#ifndef PROP3D_H
#define PROP3D_H

#include "GFX.h"
#include "Matrices.h"
#include "Obj3D.h"
#include "ShaderProgram.h"
/*

  A Prop3D object represents a unique instance of an Obj3D mesh in a Scene3D,
  with distinct position, orientation, texture and other attributes

*/


class Prop3D
{
  public:
    Prop3D();
    ~Prop3D();

    void render(Matrix4 scene_matrix);
    Obj3D* Object();
    void setObject(Obj3D* object);
    void setScale(float scale);
    void setScale(Vector3 scale);
    void setPosition(Vector3 position);
    void setDirection(Vector3 direction);
    void setShader(ShaderProgram* shader, const std::string v, const std::string vt, const std::string vn);
    void setTexture(GLuint texture);

  protected:

  private:
    Obj3D* object;
    ShaderProgram* shader;
    GLint shader_v;
    GLint shader_vt;
    GLint shader_vn;
    GLuint texture;
    Matrix4 mat;
    Vector3 pos;
    Vector3 dir;
    Vector3 scale;
    bool need_recalc;

    void recalculate();

};

#endif // PROP3D_H
