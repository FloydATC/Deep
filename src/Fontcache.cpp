#include <iostream>

#include "Fontcache.h"
#include "UTF8hack.h"

Fontcache::Fontcache()
{
  //ctor
}

Fontcache::Fontcache(TTF_Font* font)
{
  //ctor
  this->font = font;

  // Preload charmap cache
  //std::cout << "Fontcache() Preloading charmap cache" << std::endl;
  for (int cc=32; cc<128; cc++) { glyph(cc); }

  //std::cout << "Fontcache() Ready" << std::endl;
}

Fontcache::~Fontcache()
{
  //dtor
}



// https://stackoverflow.com/questions/19968705/unsigned-integer-as-utf-8-value
// replaced with EXPERIMENTAL utf8.h
/*
std::string Fontcache::UnicodeToUTF8(unsigned int codepoint)
{
    std::string out;
    //std::cout << "Fontcache::UnicodeToUTF8() cp=" << codepoint << std::endl;

    if (codepoint <= 0x7f)
        out.append(1, static_cast<char>(codepoint));
    else if (codepoint <= 0x7ff)
    {
        out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
        out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
    }
    else if (codepoint <= 0xffff)
    {
        out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
        out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
        out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
    }
    else
    {
        out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
        out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
        out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
        out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
    }

    //std::cout << "Fontcache::UnicodeToUTF8() char=" << out << std::endl;
    return out;
}
*/


GLuint Fontcache::glyph(int codepoint)
{
  const int unknown = 0x25c8;
  SDL_Surface* rgba_surface = nullptr;
  GLuint textureID;
  //std::cout << "glyph() Using cache " << &charmap << std::endl;
  if (charmap.count(codepoint)==0) {
    if (codepoint < 32) { codepoint = unknown; }
    //std::cout << "glyph() Need to render charcode " << codepoint << " using font " << &font << std::endl;
    SDL_Color fcolor = { 255, 255, 255 };
    //std::string str = UnicodeToUTF8(codepoint);
    char buf[5] = {0};
    char* err = nullptr;
    char* res = UTF8hack::append(codepoint, buf, err);
    if (strlen(res)==0) { strcpy(res, "X"); }
    //std::cout << "glyph() String representation is '" << res << "'" << std::endl;
    //SDL_Surface* surface = TTF_RenderUTF8_Solid(font, str.c_str(), fcolor);
    SDL_Surface* surface = TTF_RenderUTF8_Solid(font, res, fcolor);
    //std::cout << "glyph() Initial surface=" << surface << std::endl;
    rgba_surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
    //std::cout << "glyph() Converted surface=" << rgba_surface << std::endl;
    // This trick works for monospace fonts only
    width = rgba_surface->w;
    height = rgba_surface->h;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rgba_surface->w, rgba_surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba_surface->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);

    SDL_FreeSurface(rgba_surface);
    SDL_FreeSurface(surface);
    //if (rgba_surface == nullptr && charcode!=unknown) { rgba_surface = glyph(unknown); }
    //std::cout << "glyph() Cache charcode " << charcode << std::endl;
    //charmap.insert({charcode, rgba_surface});
    charmap.insert({codepoint, textureID});
  }
  //std::cout << "glyph() returning " << charmap[charcode] << std::endl;
  return charmap[codepoint];
}

