
CPP = /usr/bin/g++

.PHONY: all clean

		
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs)

CFLAGS := $(SDL_CFLAGS) -O3
LDFLAGS := $(SDL_LDFLAGS) -lSDL_mixer

#$(appname):
#	$(CPP) $(LDFLAGS) -o $@ $^ $(LIBS)


SRC_DIR := src
OBJ_DIR := obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS :=
CPPFLAGS := -Iinclude $(SDL_CFLAGS) -O3 -std=c++11 
CXXFLAGS := 

main.exe: $(OBJ_FILES)
	g++ $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<
   

clean:
	@rm -f obj/*.o
   