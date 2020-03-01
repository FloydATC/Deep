#ifndef PROP3D_H
#define PROP3D_H

#include <vector>

#include "GFX.h"
#include "Matrices.h"
#include "3D/Obj3D.h"
#include "3D/Camera3D.h"
#include "3D/Entity3D.h"
#include "ShaderProgram.h"
/*

  A Prop3D object represents a unique instance of an Mesh3D mesh in a Scene3D,
  with the following properties:
  - position
  - orientation
  - scale

*/


class Prop3D : public Entity3D
{
  public:
    Prop3D();
    ~Prop3D();

    void render(Camera3D camera);

    void setMesh(Mesh3D* mesh);
    Mesh3D* getMesh();

    void setScale(float scale);
    void setScale(Vector3 scale);
    Matrix4 getScaleMatrix();

    bool mouse_intersects(Vector2 mouse, Vector2 display);
    Vector2 relative_mouse_pos(Vector2 mouse, Camera3D* camera, void* scene);

    void setShader(ShaderProgram* shader);

    // Texture can be set on invididual Props,
    // and/or on the underlying shared mesh objects
    void setTexture(GLuint texture);

    // Make private once values have been verified
    std::vector<Vector3> xy_plane;
    bool xy_plane_visible();

  protected:

  private:
    Mesh3D* mesh;

    ShaderProgram* shader = nullptr;

    GLuint texture;
    bool texture_set;

    Vector3 scale;
    Matrix4 scale_matrix;

    void recalculate_matrix();
    Vector3 project(Vector3 v3, Camera3D camera);
    void recalculate_xy_plane(Camera3D camera);

    //float color[4] = { 1.0, 1.0, 1.0, 1.0 };

};


#endif // PROP3D_H