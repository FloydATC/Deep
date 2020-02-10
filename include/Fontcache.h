#ifndef FONTCACHE_H
#define FONTCACHE_H

#include <unordered_map>
#include "GFX.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>


class Fontcache
{
  public:
    Fontcache();
    Fontcache(TTF_Font* font);
    ~Fontcache();

    //SDL_Surface* glyph(int charcode);
    GLuint glyph(int charcode);
    static std::string UnicodeToUTF8(unsigned int codepoint);
    int height = -1;
    int width = -1;



  protected:

  private:
    TTF_Font* font;
    //std::unordered_map<int,SDL_Surface*> charmap;
    std::unordered_map<int,GLuint> charmap;
};

#endif // FONTCACHE_H
