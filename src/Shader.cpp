#include "Shader.h"

#include "IOFile.h"

Shader::Shader(const std::string filename, GLenum shaderType)
{
  //ctor
  this->shaderId = glCreateShader(shaderType);
  this->shaderType = shaderType;
  this->filename = filename;
  //std::cout << "Shader() created with id=" << this->shaderId << std::endl;
  this->compile();
}

Shader::~Shader()
{
  //dtor
  glDeleteShader(this->shaderId);
}


std::string Shader::type()
{
  if (this->shaderType == GL_VERTEX_SHADER) return "vertex";
  if (this->shaderType == GL_FRAGMENT_SHADER) return "fragment";
  return "unknown/invalid";
}

GLuint Shader::id()
{
  return this->shaderId;
}

void Shader::compile()
{
  IOFile file;
  glsl = file.slurp(this->filename);
  const char* c_str = glsl.c_str();

  // Compile shader
  glShaderSource(shaderId, 1, &c_str, nullptr);
  glCompileShader(shaderId);
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &this->success);
  if (!this->success) fetchError();

}

void Shader::fetchError()
{
  GLchar InfoLog[1024];
  glGetShaderInfoLog(shaderId, sizeof(InfoLog), NULL, InfoLog);
  this->error = glsl;
  this->error.append("\n");
  this->error.append("Error compiling ");
  this->error.append(this->type());
  this->error.append(" shader: ");
  this->error.append(InfoLog);
  this->error.append("\n");
}

