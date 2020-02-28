#ifndef PROP3D_H
#define PROP3D_H

#include <vector>

#include "GFX.h"
#include "Matrices.h"
#include "3D/Obj3D.h"
#include "3D/Camera3D.h"
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

    void render(Camera3D camera);
    bool mouse_intersects(Vector2 mouse, Vector2 display);
    Vector2 relative_mouse_pos(Vector2 mouse, Vector2 display);
    Obj3D* Object();
    void setObject(Obj3D* object);
    void setScale(float scale);
    void setScale(Vector3 scale);
    void setPosition(Vector3 position);
    void setDirection(Vector3 direction);
    void setShader(ShaderProgram* shader);
    void setTexture(GLuint texture);

    // Make private once values have been verified
    std::vector<Vector3> xy_plane;
    bool xy_plane_visible();

  protected:

  private:
    Obj3D* object;

    ShaderProgram* shader = nullptr;
    GLuint texture;
    Matrix4 mat;
    Vector3 pos;
    Vector3 dir;
    Vector3 scale;
    bool need_recalc;

    void recalculate_matrix();
    Vector3 project(Vector3 v3, Camera3D camera);
    void recalculate_xy_plane(Camera3D camera);

};


#endif // PROP3D_H
