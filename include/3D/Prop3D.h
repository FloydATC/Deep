#ifndef PROP3D_H
#define PROP3D_H

#include <vector>

#include "GFX.h"
#include "Matrices.h"
#include "3D/Obj3D.h"
#include "3D/Camera3D.h"
#include "3D/Entity3D.h"
#include "ShaderProgram.h"
#include "Texture.h"
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

    void render(Camera3D* camera);

    void setMesh(Mesh3D* mesh);
    Mesh3D* getMesh();

    void setScale(float scale);
    void setScale(Vector3 scale);
    Matrix4 getScaleMatrix();

    bool mouse_intersects(Vector2 mouse);
    Vector2 relative_mouse_pos(Vector2 mouse, Camera3D* camera);

    void setShader(ShaderProgram* shader);

    // Texture can be set on invididual Props,
    // and/or on the underlying shared mesh objects
    void setTexture(GLuint texture);
    void setTexture(Texture* texture);

    void setDecalTexture(GLuint texture);
    void setDecalTexture(Texture* texture) { setDecalTexture(texture->id()); }
    void setDecalPosition(Vector2 position);

    // Make private once values have been verified
    std::vector<Vector3> xy_plane;
    bool xy_plane_visible();

    void showBounds();
    void hideBounds();


  protected:

  private:
    Mesh3D* mesh;

    ShaderProgram* shader;
    Material default_material;
    Material* material;

    GLuint texture;
    bool texture_set;

    GLuint decal_texture;
    bool decal_texture_set;
    Vector2 decal_position;
    bool decal_position_set;

    bool bounds_enabled;

    Vector3 scale;
    Matrix4 scale_matrix;

    void recalculate_matrix();
    //Vector3 project(Vector3 v3, Camera3D* camera);
    void recalculate_xy_plane(Camera3D* camera);

    //float color[4] = { 1.0, 1.0, 1.0, 1.0 };

};


#endif // PROP3D_H
