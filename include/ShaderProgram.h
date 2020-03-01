#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <iostream>
#include <string>
#include "GFX.h"
#include "Matrices.h"

class ShaderProgram
{
  public:
    ShaderProgram(const std::string vs_filename, const std::string fs_filename);
    ~ShaderProgram();

    GLuint id();
    void setAttributeV(std::string name);
    void setAttributeVT(std::string name);
    void setAttributeVN(std::string name);
    void setUniformCameraMatrix(std::string name);
    void setUniformModelMatrix(std::string name);
    void setUniformColor(std::string name);
    void setUniformDebugFlag(std::string name);
    void setUniformTextureFlag(std::string name);

    void enableAttributeV();
    void enableAttributeVT();
    void enableAttributeVN();
    void disableAttributeV();
    void disableAttributeVT();
    void disableAttributeVN();
    void setAttribPointerV(GLint values, GLenum type, GLsizei typesize, GLsizei stride, GLsizei offset);
    void setAttribPointerVT(GLint values, GLenum type, GLsizei typesize, GLsizei stride, GLsizei offset);
    void setAttribPointerVN(GLint values, GLenum type, GLsizei typesize, GLsizei stride, GLsizei offset);
    void setCameraMatrix(Matrix4 matrix);
    void setModelMatrix(Matrix4 matrix);
    void setDebugFlag(bool value);
    void setTextureFlag(bool value);
    void setColor(float r, float g, float b, float a);
    void setColor(GLfloat* color);

    int success;
    std::string error;

    GLint uniform_camera_mat;
    GLint uniform_model_mat;
    GLint uniform_color;
    GLint uniform_debug_flag;
    GLint uniform_texture_flag;
    GLint vertex_v;
    GLint vertex_vt;
    GLint vertex_vn;
  protected:

  private:
    void fetchError();
    void compile();

    GLuint programId;


    std::string vs_filename;

    std::string fs_filename;
};

#endif // SHADERPROGRAM_H
