#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

#include "GFX.h"
#include "SDL_image.h"


class Texture
{
  public:
    Texture();
    ~Texture();

    GLuint id();
    bool load(std::string filename);


  protected:

  private:
    int width;
    int height;
    GLuint texture;

    SDL_Surface* load_surface(std::string filename);
    SDL_Surface* create_surface();
    SDL_Surface* convert_surface_format(SDL_Surface* surface, const SDL_PixelFormat* format);
    void flip_surface(SDL_Surface* original, SDL_Surface* flipped);
    SDL_Surface* load_and_flip(const std::string filename);

};


#endif // TEXTURE_H
