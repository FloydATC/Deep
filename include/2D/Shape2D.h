#ifndef SHAPE2D_H
#define SHAPE2D_H

#include "GFX.h"
#include "ShaderProgram.h"


class Shape2D
{
  public:
    Shape2D();
    virtual ~Shape2D();

    void setShader(ShaderProgram* shader);
    void setDimensions(int width, int height);
    void draw_untextured_vbo(int arrsize, const void* arr, GLenum type, GLsizei typesize, GLenum mode, GLsizei vertices);
    int tx(int x);
    int ty(int y);

  protected:

  private:

    int width;
    int height;

    GLint attr_vertex;
    GLint attr_uv;
    ShaderProgram* shader;

};

#endif // SHAPE2D_H
