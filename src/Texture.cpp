#include "Texture.h"

#include <iostream>

#include "SDL_image.h"


Texture::Texture()
{
  //ctor
  glGenTextures(1, &this->texture);
}

Texture::~Texture()
{
  //dtor
  glDeleteTextures(1, &this->texture);
}


GLuint Texture::id()
{
  return this->texture;
}


bool Texture::load(std::string filename)
{
  // You should probably use CSurface::OnLoad ... ;)
  //-- and make sure the Surface pointer is good!
  //std::cout<<"load_texture() load png into surface"<<std::endl;
  SDL_Surface* Surface = IMG_Load(filename.c_str());
  if(Surface) {
    //std::cout<<"load_texture() OK surface=" << Surface << " h=" << Surface->h << " w=" << Surface->w << std::endl;
  } else {
    std::cout << "Texture" << this << "::load() FAILED " << IMG_GetError() << std::endl;
    // handle error
    //running = false;
    return false;
  }

  glBindTexture(GL_TEXTURE_2D, this->texture);

  int Mode = GL_RGB;

  if(Surface->format->BytesPerPixel == 4) {
      Mode = GL_RGBA;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, Mode, Surface->w, Surface->h, 0, Mode, GL_UNSIGNED_BYTE, Surface->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Any other glTex* stuff here
  // ...

  return true;
}

