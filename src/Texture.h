// Texture.h: interface for the Texture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined TEXTURE_H
#define TEXTURE_H


#include <SDL_opengl.h>

class Texture  
{
public:
  char filename[256];
  GLuint id;

	Texture();
	virtual ~Texture();

};

#endif
