
//#include <stdio.h>
//#include <stdint.h>
//#include <stddef.h>
//#include <iostream>
//#include <cstddef>
//#include <stdlib.h>

typedef __PTRDIFF_TYPE__ ptrdiff_t;
//typedef signed long long ptrdiff_t;

//#include <stddef.h>
//#include <cstdlib>
#include <GL/glew.h>
#include <SDL_opengl.h>


//#define DEBUG_NO_VIRTUAL_MACHINES
#define DEBUG_RENDER_BOUNDING_BOXES


/*

13:36:06 [floyd@pixie:~/cpp/Deep] 0$ make
g++ -O3 -Wall -std=c++14 -m64 -g -I../../c/FunC -Iinclude -I/usr/include/libdrm -I/usr/include/SDL2 -D_REENTRANT -L/usr/lib/x86_64-linux-gnu -lSDL2  -lX11 -lSDL_mixer -lglut  -o obj/Machine.o -c src/Machine.cpp
In file included from include/GFX.h:3:0,
                 from include/Machine.h:10,
                 from src/Machine.cpp:13:
/usr/include/GL/glew.h:1676:9: error: ‘ptrdiff_t’ does not name a type
 typedef ptrdiff_t GLintptr;

*/
