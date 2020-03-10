#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <iostream>
#include <string>
#include "GFX.h"
#include "Matrices.h"
#include "Material.h"

class ShaderProgram
{
  public:
    ShaderProgram(const std::string vs_filename, const std::string fs_filename);
    ~ShaderProgram();

    GLint current();
    GLuint id();

    void setAttributeV(std::string name);
    void setAttributeVT(std::string name);
    void setAttributeVN(std::string name);

    void setUniformProjectionMatrix(std::string name);
    void setUniformViewMatrix(std::string name);
    void setUniformModelMatrix(std::string name);

    void setUniformAmbientColor(std::string name);
    void setUniformDiffuseColor(std::string name);
    void setUniformSpecularColor(std::string name);
    void setUniformEmissiveColor(std::string name);

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

    void setProjectionMatrix(Matrix4 matrix);
    void setViewMatrix(Matrix4 matrix);
    void setModelMatrix(Matrix4 matrix);

    void setDebugFlag(bool value);
    void setTextureFlag(bool value);
    void setColors(Material* material);

    void setAmbientColor(Vector4 color);
    void setDiffuseColor(Vector4 color);
    void setSpecularColor(Vector4 color);
    void setEmissiveColor(Vector4 color);

    bool hasAttributeV();
    bool hasAttributeVT();
    bool hasAttributeVN();

    int success;
    std::string error;

    GLint uniform_projection_mat;
    GLint uniform_view_mat;
    GLint uniform_model_mat;

    GLint uniform_color_a;
    GLint uniform_color_d;
    GLint uniform_color_s;
    GLint uniform_color_e;

    GLint uniform_debug_flag;
    GLint uniform_texture_flag;
    GLint vertex_v;
    GLint vertex_vt;
    GLint vertex_vn;

    std::string vs_filename;
    std::string fs_filename;

  protected:

  private:
    void fetchError();
    void compile();
    void setDefaults();

    GLuint programId;



};


std::ostream& operator <<(std::ostream& stream, const ShaderProgram* shader);

#endif // SHADERPROGRAM_H
