#ifndef MESH3D_H
#define MESH3D_H

#include <string>

#include "GFX.h"
#include "ShaderProgram.h"
#include "Material.h"

class Mesh3D
{
  public:
    // No public ctor
    virtual ~Mesh3D();

    virtual void render(ShaderProgram* shader) = 0;

    void setName(std::string name);
    std::string getName();

    void setTexture(GLuint texture);

    void setColor(float* color);
    void setColor(float r, float g, float b, float a);

    void setMaterial(Material* material);
    Material* getMaterial();
    void setBounds(Mesh3D* box);
    Mesh3D* getBounds();
    bool bounds_enabled;

    bool isEnabled();
    void show();
    void hide();

    bool debug;


  protected:
    Mesh3D();

    void initialize(ShaderProgram* shader);

    friend class Obj3DLoader;
    Material* material;
    void set_v(float* v, int num_vertices);
    void set_vt(float* v, int num_vertices);
    void set_vn(float* v, int num_vertices);

    std::string name;

    int count_v;
    int count_vt;
    int count_vn;

    GLuint vao;
    GLuint vbo_v;
    GLuint vbo_vt;
    GLuint vbo_vn;
    GLuint texture;

    bool texture_set;
    bool render_enabled;

    Mesh3D* bounds;

    void bind_vao();
    void unbind_vao();
    void bind_vbo(GLuint vbo);

    bool initialized;

    GLfloat color[4] = { 1.0, 1.0, 1.0, 1.0 };

  private:
};

#endif // MESH3D_H
