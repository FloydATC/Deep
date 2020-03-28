#ifndef OVERLAY2D_H
#define OVERLAY2D_H

#include "2D/Polygon2D.h"
#include "2D/Shape2D.h"
#include "GFX.h"
#include "Matrices.h"
#include "ShaderProgram.h"

class Overlay2D
{
  public:
    Overlay2D();
    ~Overlay2D();

    void pre_render();
    void post_render();
    void setShader(ShaderProgram* shader);
    void setDimensions(int width, int height);

    void draw_polygon(Vector2 v1, Vector2 v2, Vector2 v3);
    void draw_quad(Vector2 v1, Vector2 v2, Vector2 v3, Vector2 v4);

  protected:

  private:
    int width;
    int height;

    Matrix4 mat;
    ShaderProgram* shader = nullptr;

    GLfloat color[4] = { 0.2f, 0.2f, 1.0f, 0.5f };

    GLint attr_vertex;
    GLint attr_uv;
    GLuint uniform_ortho;
    GLuint uniform_color;
    GLuint vao; // Vertex Array Object
    GLuint vbo; // Vertex Buffer Object

    void pre_draw(Shape2D* shape);
    void post_draw();

    void bind_vao();
    void unbind_vao();
    void bind_vbo(GLuint vbo);

};

#endif // OVERLAY2D_H
