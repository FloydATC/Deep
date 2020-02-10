
CPP = /usr/bin/g++

.PHONY: all clean

		
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --static-libs)

GL_CFLAGS = -I/usr/include/libdrm
GL_LDFLAGS = -lGLEW -lGLU -lGL
GL_LIBS = -L/usr/lib/x86_64-linux-gnu/

FUNC_LIBS = -L../../c/FunC/bin/Debug



SRC_DIR := src
OBJ_DIR := obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
CFLAGS := -O3 -Wall -std=c++11 -m64 -g -I../../c/FunC -Iinclude $(GL_CFLAGS) $(SDL_CFLAGS)
LDFLAGS := $(FUNC_LIBS) $(GL_LIBS) $(SDL_LDFLAGS) $(GL_LDFLAGS) -lX11 -lSDL_ttf -lSDL_mixer -lSDL_image -lFunC

main.exe: $(OBJ_FILES)
	g++ -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CFLAGS) $(LDFLAGS) -o $@ -c $<
   

clean:
	@rm -f obj/*.o src/*~ include/*~ *~
	




