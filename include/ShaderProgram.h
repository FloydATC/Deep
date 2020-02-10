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
    void compile();

    int success;
    std::string error;

  protected:

  private:
    void fetchError();

    GLuint programId;
    std::string vs_filename;
    std::string fs_filename;
};

#endif // SHADERPROGRAM_H
