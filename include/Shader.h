#ifndef SHADER_H
#define SHADER_H

#include <iostream>
#include <string>
#include "GFX.h"

class Shader
{
  public:
    Shader(const std::string filename, GLenum shaderType);
    ~Shader();

    std::string type();
    GLuint id();
    void compile();
    std::string getFilename();

    GLint success;
    std::string error;

  protected:

  private:
    void fetchError();

    GLuint shaderId;
    std::string filename;
    std::string glsl;
    GLenum shaderType;
};

#endif // SHADER_H
