#include "ShaderProgram.h"

#include "Shader.h"

ShaderProgram::ShaderProgram(const std::string vs_filename, const std::string fs_filename)
{
  //ctor
  this->programId = glCreateProgram();
  this->vs_filename = vs_filename;
  this->fs_filename = fs_filename;
  //std::cout << "shaderProgram() created with id=" << this->programId << std::endl;
  this->compile();
}

ShaderProgram::~ShaderProgram()
{
  //dtor
  glDeleteProgram(this->programId);
}


GLuint ShaderProgram::id()
{
  return programId;
}



void ShaderProgram::compile()
{
  Shader* vs = new Shader(vs_filename, GL_VERTEX_SHADER);
  if (!vs->success) {
    std::cerr << vs->error;
  }

  Shader* fs = new Shader(fs_filename, GL_FRAGMENT_SHADER);
  if (!fs->success) {
    std::cerr << fs->error;
  }


  // Link compiled shaders into a shaderprogram
  glAttachShader(programId, vs->id());
  glAttachShader(programId, fs->id());
  glLinkProgram(programId);
  glGetProgramiv(programId, GL_LINK_STATUS, (int *)&this->success);
  if(!success) fetchError();
//  {
//    int maxLength = 1024;
//
//    /* Noticed that glGetProgramiv is used to get the length for a shader program, not glGetShaderiv. */
//    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &maxLength);
//
//    /* The maxLength includes the NULL character */
//    GLchar* InfoLog = (char *)malloc(maxLength);
//
//    /* Notice that glGetProgramInfoLog, not glGetShaderInfoLog. */
//    glGetProgramInfoLog(programId, maxLength, &maxLength, InfoLog);
//
//    std::cerr << "Error linking shader program: " << InfoLog << std::endl;
//    free(InfoLog);
//  }

  //check_gl("linking shaders");

  // Dispose of the shader objects (they have been compiled into shaderProgram now)
  glDetachShader(programId, vs->id());
  glDetachShader(programId, fs->id());

  // No need to keep these around anymore
  delete vs;
  delete fs;
}

void ShaderProgram::fetchError()
{
  GLchar InfoLog[1024];
  glGetProgramInfoLog(programId, sizeof(InfoLog), NULL, InfoLog);
  //glGetShaderInfoLog(shaderId, sizeof(InfoLog), NULL, InfoLog);
  this->error.append("Error linking shader program: ");
  this->error.append(InfoLog);
  this->error.append("\n");
}
