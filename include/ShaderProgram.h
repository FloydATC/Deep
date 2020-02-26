#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <iostream>
#include <string>
#include "GFX.h"

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
    void setUniformDebugFlag(std::string name);

    int success;
    std::string error;

    GLuint uniform_camera_mat;
    GLuint uniform_model_mat;
    GLuint uniform_debug_flag;
    GLuint vertex_v;
    GLuint vertex_vt;
    GLuint vertex_vn;
  protected:

  private:
    void fetchError();
    void compile();

    GLuint programId;


    std::string vs_filename;

    std::string fs_filename;
};

#endif // SHADERPROGRAM_H
