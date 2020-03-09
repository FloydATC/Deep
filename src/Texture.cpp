#include "Texture.h"

#include <iostream>



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


SDL_Surface* Texture::load_surface(std::string filename)
{
  SDL_Surface* result = IMG_Load(filename.c_str());
  if(!result) {
    std::cout << "Texture" << this << "::load_surface() failed: " << IMG_GetError() << std::endl;
    return nullptr;
  }
  this->width = result->w;
  this->height = result->h;
  return result;
}


SDL_Surface* Texture::create_surface()
{
  SDL_Surface* result = SDL_CreateRGBSurfaceWithFormat(
    0,
    this->width,
    this->height,
    32,
    SDL_PIXELFORMAT_RGBA32
  );
  if(!result) {
    std::cout << "Texture" << this << "::create_surface() failed: " << IMG_GetError() << std::endl;
    return nullptr;
  }
  return result;
}


SDL_Surface* Texture::convert_surface_format(SDL_Surface* surface, const SDL_PixelFormat* format)
{
  SDL_Surface* result = SDL_ConvertSurface(surface, format, 0);
  if(!result) {
    std::cout << "Texture" << this << "::load() format conversion : " << IMG_GetError() << std::endl;
    return nullptr;
  }
  return result;
}

void Texture::flip_surface(SDL_Surface* original, SDL_Surface* flipped)
{
  SDL_Rect srcline;
  SDL_Rect dstline;

  srcline.x = 0;
  srcline.w = this->width;
  srcline.h = 1;

  dstline.x = 0;
  dstline.w = this->width;
  dstline.h = 1;

  // Copy each scanline
  for (int i=0; i<this->height; i++) {
    srcline.y = i;
    dstline.y = this->height-i-1;
    SDL_BlitSurface(original, &srcline, flipped, &dstline);
  }
}


SDL_Surface* Texture::load_and_flip(const std::string filename)
{
  SDL_Surface* original = load_surface(filename.c_str());
  if (original == nullptr) return nullptr;

  SDL_Surface* flipped = create_surface();
  if (flipped == nullptr) return nullptr;

  original = convert_surface_format(original, flipped->format);
  if (original == nullptr) return nullptr;

  flip_surface(original, flipped);
  SDL_FreeSurface(original);
  return flipped;
}

bool Texture::load(std::string filename)
{
  SDL_Surface* flipped = load_and_flip(filename);
  if (flipped == nullptr) return false;

  glBindTexture(GL_TEXTURE_2D, this->texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, flipped->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Any other glTex* stuff here
  // ...

  SDL_FreeSurface(flipped);
  return true;
}

