#ifndef OBJ3D_H
#define OBJ3D_H

#include <vector>
#include <string>

#include "GFX.h"
#include <SDL.h>

#include "Box3D.h"

/*

  An Obj3D encapsulates the contents of a Wavefront .OBJ file
  represented by one or more vertex array buffers:

  - vertex positions
  - uv texture coordinates (optional)
  - vertex normals (optional)

  An Obj3D object may consist of one or more subobjects
  Quad faces are automatically converted to triangle polygons

*/


class Obj3D
{
  public:
    Obj3D();
    ~Obj3D();

    void set_subobjects(std::vector<int> start, std::vector<int> length);
    void set_bounding_boxes(std::vector<Box3D*> bounding_box);

    void set_v(float* v, int length);
    void set_vt(float* v, int length);
    void set_vn(float* v, int length);

    void set_shader_v(GLint attr);
    void set_shader_vt(GLint attr);
    void set_shader_vn(GLint attr);

    void render(int subobject);
    Box3D* bounding_box(int subobject);

    int subobjects;

  protected:

  private:
    int vertices;
    std::vector<int> subobject_start;
    std::vector<int> subobject_length;
    std::vector<Box3D*> bounding_boxes;

    void bind_vao();
    void unbind_vao();
    void bind_vbo(GLuint vbo);

    GLuint vao;
    GLuint vbo_v;
    GLuint vbo_vt;
    GLuint vbo_vn;
    GLint shader_v;
    GLint shader_vt;
    GLint shader_vn;
};

#endif // OBJ3D_H
