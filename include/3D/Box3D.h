#ifndef BOX3D_H
#define BOX3D_H

#include <vector>

#include "GFX.h"
#include "Matrices.h" // Vector3

class Box3D
{
  public:
    Box3D();
    ~Box3D();

    void extend(Vector3 vertex);
    void set_shader_v(GLint attr);
    void finalize();
    void render();

    void setName(std::string name);
    std::string getName();

  protected:

  private:
    std::vector<GLfloat> vertices;
    std::vector<GLuint> index = {
      0,1, 2,3, 0,2, 1,3,
      4,5, 6,7, 4,6, 5,7,
      0,4, 1,5, 2,6, 3,7
    };
    Vector3 minimum = Vector3(0,0,0);
    Vector3 maximum = Vector3(0,0,0);
    bool first = true;
    bool finalized = false;
    std::string name;

    GLuint vao; // Vertex Array Object
    GLuint vbo; // Vertex Buffer Object
    GLuint ibo; // Index Buffer Object
    GLint shader_v;

    void set_v();
    void set_i();

    void bind_vao();
    void unbind_vao();
    void bind_vbo(GLuint vbo);
    void bind_ibo(GLuint ibo);

};

#endif // BOX3D_H
