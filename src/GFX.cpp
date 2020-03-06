#include <iostream>

#include "GFX.h"


void dumpVertexArrayState()
{
  size_t bufsize = 64;

  std::cout << "GFX::dumpVertexArrayState()" << std::endl;


  GLint vao_id;
  char vao_name[bufsize+1];
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vao_id);
  glGetObjectLabel(GL_VERTEX_ARRAY, vao_id, bufsize, NULL, vao_name);
  std::cout << "  VAO: " << vao_id << " (" << vao_name << ")" << std::endl;

  // https://www.khronos.org/registry/OpenGL-Refpages/es2.0/xhtml/glGet.xml
  // glGet
  // GL_MAX_VERTEX_ATTRIBS

  // GL_TEXTURE_BINDING_2D
  GLint texture_id;
  char texture_name[bufsize+1];
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture_id);
  glGetObjectLabel(GL_TEXTURE, texture_id, bufsize, NULL, texture_name);
  std::cout << "  Texture: " << texture_id << " (" << texture_name << ")" << std::endl;

}
