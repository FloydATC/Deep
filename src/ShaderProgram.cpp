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
  this->uniform_projection_mat = -1;
  this->uniform_view_mat = -1;
  this->uniform_model_mat = -1;
  this->uniform_color_a = -1;
  this->uniform_color_d = -1;
  this->uniform_color_s = -1;
  this->uniform_color_e = -1;
  this->uniform_debug_flag = -1;
  this->uniform_texture_flag = -1;

  this->setDefaults();
}

ShaderProgram::~ShaderProgram()
{
  //dtor
  glDeleteProgram(this->programId);
  //std::cout << "shaderProgram() destroyed" << std::endl;
}


GLint ShaderProgram::current()
{
  GLint current;
  glGetIntegerv(GL_CURRENT_PROGRAM, &current);
  return current;
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


void ShaderProgram::setDefaults()
{
  this->setAttributeV("attr_v");
  this->setAttributeVT("attr_vt");
  this->setAttributeVN("attr_vn");

  this->setUniformProjectionMatrix("projection");
  this->setUniformViewMatrix("view");
  this->setUniformModelMatrix("model");

  this->setUniformAmbientColor("color_a");
  this->setUniformDiffuseColor("color_d");
  this->setUniformSpecularColor("color_s");
  this->setUniformEmissiveColor("color_e");

  this->setUniformDebugFlag("is_debug");
  this->setUniformTextureFlag("use_texture");
}

void ShaderProgram::setAttributeV(std::string name)
{
  this->vertex_v = glGetAttribLocation(this->programId, name.c_str());
}

void ShaderProgram::setAttributeVT(std::string name)
{
  this->vertex_vt = glGetAttribLocation(this->programId, name.c_str());
}

void ShaderProgram::setAttributeVN(std::string name)
{
  this->vertex_vn = glGetAttribLocation(this->programId, name.c_str());
}


void ShaderProgram::setUniformProjectionMatrix(std::string name)
{
  this->uniform_projection_mat = glGetUniformLocation(this->programId, name.c_str());
}

void ShaderProgram::setUniformViewMatrix(std::string name)
{
  this->uniform_view_mat = glGetUniformLocation(this->programId, name.c_str());
}

void ShaderProgram::setUniformModelMatrix(std::string name)
{
  this->uniform_model_mat = glGetUniformLocation(this->programId, name.c_str());
}


void ShaderProgram::setUniformAmbientColor(std::string name)
{
  this->uniform_color_a = glGetUniformLocation(this->programId, name.c_str());
}


void ShaderProgram::setUniformDiffuseColor(std::string name)
{
  this->uniform_color_d = glGetUniformLocation(this->programId, name.c_str());
}


void ShaderProgram::setUniformSpecularColor(std::string name)
{
  this->uniform_color_s = glGetUniformLocation(this->programId, name.c_str());
}


void ShaderProgram::setUniformEmissiveColor(std::string name)
{
  this->uniform_color_e = glGetUniformLocation(this->programId, name.c_str());
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


void ShaderProgram::setProjectionMatrix(Matrix4 matrix)
{
  if (this->uniform_projection_mat == -1) {
    std::cerr << "ShaderProgram::setProjectionMatrix() uniform not set" << std::endl;
    return;
  }
  glUniformMatrix4fv(this->uniform_projection_mat, 1, GL_FALSE, matrix.get());
}

void ShaderProgram::setViewMatrix(Matrix4 matrix)
{
  if (this->uniform_view_mat == -1) {
    std::cerr << "ShaderProgram::setViewMatrix() uniform not set" << std::endl;
    return;
  }
  glUniformMatrix4fv(this->uniform_view_mat, 1, GL_FALSE, matrix.get());
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

void ShaderProgram::setColors(Material* material)
{
  if (this->uniform_color_a != -1) glUniform4fv(this->uniform_color_a, 1, material->getAmbientColor().data);
  if (this->uniform_color_d != -1) glUniform4fv(this->uniform_color_d, 1, material->getDiffuseColor().data);
  if (this->uniform_color_s != -1) glUniform4fv(this->uniform_color_s, 1, material->getSpecularColor().data);
  if (this->uniform_color_e != -1) glUniform4fv(this->uniform_color_e, 1, material->getEmissiveColor().data);
}


void ShaderProgram::setAmbientColor(Vector4 color)
{
  if (this->uniform_color_a == -1) {
    std::cerr << "ShaderProgram::setAmbientColor() uniform not set" << std::endl;
    return;
  }
  glUniform4fv(this->uniform_color_a, 1, color.data);
}
void ShaderProgram::setDiffuseColor(Vector4 color)
{
  if (this->uniform_color_d == -1) {
    std::cerr << "ShaderProgram::setDiffuseColor() uniform not set" << std::endl;
    return;
  }
  glUniform4fv(this->uniform_color_d, 1, color.data);
}
void ShaderProgram::setSpecularColor(Vector4 color)
{
  if (this->uniform_color_s == -1) {
    std::cerr << "ShaderProgram::setSpecularColor() uniform not set" << std::endl;
    return;
  }
  glUniform4fv(this->uniform_color_s, 1, color.data);
}
void ShaderProgram::setEmissiveColor(Vector4 color)
{
  if (this->uniform_color_e == -1) {
    std::cerr << "ShaderProgram::setEmissiveColor() uniform not set" << std::endl;
    return;
  }
  glUniform4fv(this->uniform_color_e, 1, color.data);
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


void ShaderProgram::disableAttributeV()
{
  if (this->vertex_v == -1) {
    std::cerr << "ShaderProgram::disableAttributeV() attribute not set" << std::endl;
    return;
  }
  glDisableVertexAttribArray(this->vertex_v);
}

void ShaderProgram::disableAttributeVT()
{
  if (this->vertex_vt == -1) {
    std::cerr << "ShaderProgram::disableAttributeVT() attribute not set" << std::endl;
    return;
  }
  glDisableVertexAttribArray(this->vertex_vt);
}

void ShaderProgram::disableAttributeVN()
{
  if (this->vertex_vn == -1) {
    std::cerr << "ShaderProgram::disableAttributeVN() attribute not set" << std::endl;
    return;
  }
  glDisableVertexAttribArray(this->vertex_vn);
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


bool ShaderProgram::hasAttributeV()
{
  return this->vertex_v != -1;
}

bool ShaderProgram::hasAttributeVT()
{
  return this->vertex_vt != -1;
}

bool ShaderProgram::hasAttributeVN()
{
  return this->vertex_vn != -1;
}


std::ostream& operator <<(std::ostream& stream, const ShaderProgram* shader) {
  if (shader == nullptr) {
    stream << "<no shader>";
    return stream;
  } else {
    stream << "<ShaderProgram " << shader->vs_filename << " + " << shader->fs_filename << ">";
    return stream;
  }
}

