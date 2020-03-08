#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

#include "GFX.h"

class Texture
{
  public:
    Texture();
    ~Texture();

    GLuint id();
    bool load(std::string filename);


  protected:

  private:
    GLuint texture;

};


#endif // TEXTURE_H
