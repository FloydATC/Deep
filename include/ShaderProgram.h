#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <unordered_map>

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

    void setColors(Material* material);

    int success;
    std::string error;

    // Vertex attribute methods
    void mapAttribute(std::string name, std::string alias);
    bool hasAttribute(std::string name);
    void enableAttribute(std::string name);
    void disableAttribute(std::string name);
    void setAttributePointer(std::string name, GLint values, GLenum type, GLsizei typesize, GLsizei stride, GLsizei offset);

    // Shader uniform methods
    void mapUniform(std::string name, std::string alias);
    bool hasUniform(std::string name);
    void setUniformBoolean(std::string name, bool value);
    void setUniformInteger(std::string name, int value);
    void setUniformVector2(std::string name, Vector2 value);
    void setUniformVector3(std::string name, Vector3 value);
    void setUniformVector4(std::string name, Vector4 value);
    void setUniformMatrix4(std::string name, Matrix4 value);

    std::unordered_map<std::string,GLint> attribute;
    std::unordered_map<std::string,GLint> uniform;


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
