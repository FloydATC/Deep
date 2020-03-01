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

  // Uninitialized values = not found
  this->vertex_v = -1;
  this->vertex_vt = -1;
  this->vertex_vn = -1;
  this->uniform_camera_mat = -1;
  this->uniform_model_mat = -1;
  this->uniform_color = -1;
  this->uniform_debug_flag = -1;
  this->uniform_texture_flag = -1;
}

ShaderProgram::~ShaderProgram()
{
  //dtor
  glDeleteProgram(this->programId);
  //std::cout << "shaderProgram() destroyed" << std::endl;
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
  //if(success) std::cout << "ShaderProgram " << programId << " successfully linked" << std::endl;
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


void ShaderProgram::setAttributeV(std::string name)
{
  this->vertex_v = glGetAttribLocation(this->programId, name.c_str());
  if (this->vertex_v == -1) std::cerr << "ShaderProgram::setAttributeV(" << name << ") name not found" << std::endl;
}

void ShaderProgram::setAttributeVT(std::string name)
{
  this->vertex_vt = glGetAttribLocation(this->programId, name.c_str());
  if (this->vertex_vt == -1) std::cerr << "ShaderProgram::setAttributeVT(" << name << ") name not found" << std::endl;
}

void ShaderProgram::setAttributeVN(std::string name)
{
  this->vertex_vn = glGetAttribLocation(this->programId, name.c_str());
  if (this->vertex_vn == -1) std::cerr << "ShaderProgram::setAttributeVN(" << name << ") name not found" << std::endl;
}


void ShaderProgram::setUniformCameraMatrix(std::string name)
{
  this->uniform_camera_mat = glGetUniformLocation(this->programId, name.c_str());
  if (this->uniform_camera_mat == -1) std::cerr << "ShaderProgram::setUniformCameraMatrix(" << name << ") name not found" << std::endl;
}

void ShaderProgram::setUniformModelMatrix(std::string name)
{
  this->uniform_model_mat = glGetUniformLocation(this->programId, name.c_str());
  if (this->uniform_model_mat == -1) std::cerr << "ShaderProgram::setUniformModelMatrix(" << name << ") name not found" << std::endl;
}

void ShaderProgram::setUniformColor(std::string name)
{
  this->uniform_color = glGetUniformLocation(this->programId, name.c_str());
  if (this->uniform_color == -1) std::cerr << "ShaderProgram::setUniformColor(" << name << ") name not found" << std::endl;
}

void ShaderProgram::setUniformDebugFlag(std::string name)
{
  this->uniform_debug_flag = glGetUniformLocation(this->programId, name.c_str());
  if (this->uniform_debug_flag == -1) std::cerr << "ShaderProgram::setUniformDebugFlag(" << name << ") name not found" << std::endl;
}

void ShaderProgram::setUniformTextureFlag(std::string name)
{
  this->uniform_texture_flag = glGetUniformLocation(this->programId, name.c_str());
  if (this->uniform_texture_flag == -1) std::cerr << "ShaderProgram::setUniformTextureFlag(" << name << ") name not found" << std::endl;
}


void ShaderProgram::setCameraMatrix(Matrix4 matrix)
{
  if (this->uniform_camera_mat == -1) {
    std::cerr << "ShaderProgram::setCameraMatrix() uniform not set" << std::endl;
    return;
  }
  glUniformMatrix4fv(this->uniform_camera_mat, 1, GL_FALSE, matrix.get());
}

void ShaderProgram::setModelMatrix(Matrix4 matrix)
{
  if (this->uniform_model_mat == -1) {
    std::cerr << "ShaderProgram::setModelMatrix() uniform not set" << std::endl;
    return;
  }
  glUniformMatrix4fv(this->uniform_model_mat, 1, GL_FALSE, matrix.get());
}

void ShaderProgram::setDebugFlag(bool value)
{
  if (this->uniform_debug_flag == -1) {
    std::cerr << "ShaderProgram::setDebugFlag() uniform not set" << std::endl;
    return;
  }
  glUniform1i(this->uniform_debug_flag, (value ? 1 : 0));
}

void ShaderProgram::setTextureFlag(bool value)
{
  if (this->uniform_texture_flag == -1) {
    std::cerr << "ShaderProgram::setTextureFlag() uniform not set" << std::endl;
    return;
  }
  glUniform1i(this->uniform_texture_flag, (value ? 1 : 0));
}

void ShaderProgram::setColor(float r, float g, float b, float a)
{
  GLfloat color[4] = { r, g, b, a };
  if (this->uniform_color == -1) {
    std::cerr << "ShaderProgram::setColor() uniform not set" << std::endl;
    return;
  }
  glUniform4fv(this->uniform_color, 1, color);
}

void ShaderProgram::setColor(GLfloat* color)
{
  if (this->uniform_color == -1) {
    std::cerr << "ShaderProgram::setColor() uniform not set" << std::endl;
    return;
  }
  glUniform4fv(this->uniform_color, 1, color);
}

void ShaderProgram::enableAttributeV()
{
  if (this->vertex_v == -1) {
    std::cerr << "ShaderProgram::enableAttributeV() attribute not set" << std::endl;
    return;
  }
  glEnableVertexAttribArray(this->vertex_v);
}

void ShaderProgram::enableAttributeVT()
{
  if (this->vertex_vt == -1) {
    std::cerr << "ShaderProgram::enableAttributeVT() attribute not set" << std::endl;
    return;
  }
  glEnableVertexAttribArray(this->vertex_vt);
}

void ShaderProgram::enableAttributeVN()
{
  if (this->vertex_vn == -1) {
    std::cerr << "ShaderProgram::enableAttributeVN() attribute not set" << std::endl;
    return;
  }
  glEnableVertexAttribArray(this->vertex_vn);
}


void ShaderProgram::setAttribPointerV(GLint values, GLenum type, GLsizei typesize, GLsizei stride, GLsizei offset)
{
  if (this->vertex_v == -1) {
    std::cerr << "ShaderProgram::setAttribPointerV() attribute not set" << std::endl;
    return;
  }
  glVertexAttribPointer(this->vertex_v, values, type, GL_FALSE, stride*typesize, (char*) NULL+offset*typesize);
}

void ShaderProgram::setAttribPointerVT(GLint values, GLenum type, GLsizei typesize, GLsizei stride, GLsizei offset)
{
  if (this->vertex_vt == -1) {
    std::cerr << "ShaderProgram::setAttribPointerVT() attribute not set" << std::endl;
    return;
  }
  glVertexAttribPointer(this->vertex_vt, values, type, GL_FALSE, stride*typesize, (char*) NULL+offset*typesize);
}

void ShaderProgram::setAttribPointerVN(GLint values, GLenum type, GLsizei typesize, GLsizei stride, GLsizei offset)
{
  if (this->vertex_v == -1) {
    std::cerr << "ShaderProgram::setAttribPointerVN() attribute not set" << std::endl;
    return;
  }
  glVertexAttribPointer(this->vertex_vn, values, type, GL_FALSE, stride*typesize, (char*) NULL+offset*typesize);
}

