#ifndef MESH3D_H
#define MESH3D_H

#include <string>

#include "GFX.h"
#include "ShaderProgram.h"


class Mesh3D
{
  public:
    // No public ctor
    virtual ~Mesh3D();

    virtual void render(ShaderProgram* shader) = 0;

    void setName(std::string name);
    std::string getName();


  protected:
    Mesh3D();

    void initialize(ShaderProgram* shader);

    friend class Obj3DLoader;
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

    void bind_vao();
    void unbind_vao();
    void bind_vbo(GLuint vbo);

    bool initialized;

  private:
};

#endif // MESH3D_H
