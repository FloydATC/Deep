#include "ShaderProgram.h"

#include "Shader.h"

//#define DEBUG_TRACE_SHADER
//#define DEBUG_TRACE_SHADER_VERBOSE


ShaderProgram::ShaderProgram(const std::string vs_filename, const std::string fs_filename)
{
  //ctor
  this->programId = glCreateProgram();
  this->vs_filename = vs_filename;
  this->fs_filename = fs_filename;
#ifdef DEBUG_TRACE_SHADER
  std::cout << "ShaderProgram() created: " << this << " id=" << this->programId << std::endl;
#endif
  this->compile();

  this->setDefaults();
}

ShaderProgram::~ShaderProgram()
{
  //dtor
  glDeleteProgram(this->programId);
#ifdef DEBUG_TRACE_SHADER
  std::cout << "ShaderProgram() destroyed: " << this << std::endl;
#endif
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
#ifdef DEBUG_TRACE_SHADER
  std::cout << "ShaderProgram::compile() vertex shader '" << vs_filename << "'" << std::endl;
#endif
  Shader* vs = new Shader(vs_filename, GL_VERTEX_SHADER);
  if (!vs->success) {
    std::cerr << vs->error;
  }

#ifdef DEBUG_TRACE_SHADER
  std::cout << "ShaderProgram::compile() fragment shader '" << vs_filename << "'" << std::endl;
#endif
  Shader* fs = new Shader(fs_filename, GL_FRAGMENT_SHADER);
  if (!fs->success) {
    std::cerr << fs->error;
  }


#ifdef DEBUG_TRACE_SHADER
  std::cout << "ShaderProgram::compile() link " << this << std::endl;
#endif
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
#ifdef DEBUG_TRACE_SHADER
  std::cout << "ShaderProgram::compile() finished " << this << std::endl;
#endif
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
  // Try mapping all names 1:1

  this->mapAttribute("attr_v", "attr_v");
  this->mapAttribute("attr_vt", "attr_vt");
  this->mapAttribute("attr_vn", "attr_vn");

  this->mapUniform("projection", "projection");
  this->mapUniform("view", "view");
  this->mapUniform("model", "model");

  this->mapUniform("color_a", "color_a");
  this->mapUniform("color_d", "color_d");
  this->mapUniform("color_s", "color_s");
  this->mapUniform("color_e", "color_e");

  this->mapUniform("is_debug", "is_debug");
  this->mapUniform("use_texture", "use_texture");
}


void ShaderProgram::setColors(Material* material)
{
  if (hasUniform("color_a")) setUniformVector4("color_a", material->getAmbientColor());
  if (hasUniform("color_d")) setUniformVector4("color_d", material->getDiffuseColor());
  if (hasUniform("color_s")) setUniformVector4("color_s", material->getSpecularColor());
  if (hasUniform("color_e")) setUniformVector4("color_e", material->getEmissiveColor());
}




// Vertex attribute methods

void ShaderProgram::mapAttribute(std::string name, std::string alias)
{
  GLint position = glGetAttribLocation(this->programId, alias.c_str());
  if (position != -1) this->attribute.insert({ name, position });
}

bool ShaderProgram::hasAttribute(std::string name)
{
  return this->attribute.count(name) > 0;
}

void ShaderProgram::enableAttribute(std::string name)
{
  if (hasAttribute(name)) {
    glEnableVertexAttribArray(this->attribute.at(name));
  } else {
    std::cerr << "ShaderProgram::enableAttribute() shader=" << this << " has no attribute '" << name << "'" << std::endl;
  }
}

void ShaderProgram::disableAttribute(std::string name)
{
  if (hasAttribute(name)) {
    glDisableVertexAttribArray(this->attribute.at(name));
  } else {
    std::cerr << "ShaderProgram::disableAttribute() shader=" << this << " has no attribute '" << name << "'" << std::endl;
  }
}

void ShaderProgram::setAttributePointer(std::string name, GLint values, GLenum type, GLsizei typesize, GLsizei stride, GLsizei offset)
{
  if (hasAttribute(name)) {
    glVertexAttribPointer(
      this->attribute.at(name),     // index
      values,                       // components per vertex
      type,                         // type enum
      GL_FALSE,                     // normalized
      stride*typesize,              // stride
      (char*) NULL+offset*typesize  // offset
    );
  } else {
    std::cerr << "ShaderProgram::setAttributePointer() shader=" << this << " has no attribute '" << name << "'" << std::endl;
  }
}



// Shader uniform methods

void ShaderProgram::mapUniform(std::string name, std::string alias)
{
  GLint position = glGetUniformLocation(this->programId, alias.c_str());
  if (position != -1) this->uniform.insert({ name, position });
}

bool ShaderProgram::hasUniform(std::string name)
{
  return this->uniform.count(name) > 0;
}

void ShaderProgram::setUniformBoolean(std::string name, bool value)
{
#ifdef DEBUG_TRACE_SHADER_VERBOSE
  std::cout << "ShaderProgram::setUniform(boolean) shader=" << this << " uniform=" << name << " value=" << value << std::endl;
#endif
  if (hasUniform(name)) {
    glUniform1i(this->uniform.at(name), (value ? 1 : 0));
  } else {
    std::cerr << "ShaderProgram::setUniform() shader=" << this << " has no uniform '" << name << "'" << std::endl;
  }
}

void ShaderProgram::setUniformInteger(std::string name, int value)
{
#ifdef DEBUG_TRACE_SHADER
  std::cout << "ShaderProgram::setUniform(integer) shader=" << this << " uniform=" << name << " value=" << value << std::endl;
#endif
  if (hasUniform(name)) {
    glUniform1i(this->uniform.at(name), value);
  } else {
    std::cerr << "ShaderProgram::setUniform() shader=" << this << " has no uniform '" << name << "'" << std::endl;
  }
}

void ShaderProgram::setUniformVector2(std::string name, Vector2 value)
{
  if (hasUniform(name)) {
    glUniform2fv(this->uniform.at(name), 1, value.data);
  } else {
    std::cerr << "ShaderProgram::setUniform() shader=" << this << " has no uniform '" << name << "'" << std::endl;
  }
}

void ShaderProgram::setUniformVector3(std::string name, Vector3 value)
{
  if (hasUniform(name)) {
    glUniform3fv(this->uniform.at(name), 1, value.data);
  } else {
    std::cerr << "ShaderProgram::setUniform() shader=" << this << " has no uniform '" << name << "'" << std::endl;
  }
}

void ShaderProgram::setUniformVector4(std::string name, Vector4 value)
{
  if (hasUniform(name)) {
    glUniform4fv(this->uniform.at(name), 1, value.data);
  } else {
    std::cerr << "ShaderProgram::setUniform() shader=" << this << " has no uniform '" << name << "'" << std::endl;
  }
}

void ShaderProgram::setUniformMatrix4(std::string name, Matrix4 value)
{
  if (hasUniform(name)) {
    glUniformMatrix4fv(this->uniform.at(name), 1, GL_FALSE, value.get());
  } else {
    std::cerr << "ShaderProgram::setUniform() shader=" << this << " has no uniform '" << name << "'" << std::endl;
  }
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

