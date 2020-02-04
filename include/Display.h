#ifndef DISPLAY_H
#define DISPLAY_H

#include <GL/glew.h>
#include <SDL_opengl.h>

#include "Matrices.h"
#include "Fontcache.h"


class Display
{
  public:
    Display();
    Display(GLuint shaderID, Fontcache font);
    ~Display();

    void reset();
    void check_gl(std::string when);
    GLuint prepare_vao();
    GLuint prepare_texture(GLsizei width, GLsizei height);
    GLuint prepare_framebuffer(GLuint textureID);
    bool pre_render();
    bool post_render();

    // Flip vertical axis when drawing
    inline int tx(int x) { return x; }
    inline int ty(int y) { return height-y; }

    void draw_rect(int x, int y, int w, int h);
    void draw_area(int x, int y, int w, int h);
    void draw_poly(int x1, int y1, int x2, int y2, int x3, int y3);
    void draw_line(int x1, int y1, int x2, int y2);
    void draw_circle(int x, int y, int r);
    void draw_disc(int x, int y, int r);
    void draw_point(int x, int y);
    void print(const char* str);
//    void draw_surface(int row, int col, float r, float g, float b, SDL_Surface* surface);
    void draw_surface(int row, int col, float r, float g, float b, GLuint surface);
    void cursor_up();
    void cursor_down();
    void cursor_left();
    void cursor_right();
    void cls();
    void cls(int r1, int c1, int r2, int c2);
    void cls(int r1, int c1, int r2, int c2, int codepoint);
    void blit_text(int sc1, int sr1, int sc2, int sr2, int dc1, int dr1, int dc2, int dr2);
    void scroll_up();
    void scroll_up(int r1, int c1, int r2, int c2);
    void scroll_down();
    void scroll_down(int r1, int c1, int r2, int c2);
    void scroll_left();
    void scroll_left(int r1, int c1, int r2, int c2);
    void scroll_right();
    void scroll_right(int r1, int c1, int r2, int c2);
    void hide_cursor();
    void show_cursor();
    void show_cursor(bool force);
    void enable_autoscroll();
    void disable_autoscroll();
    void set_fgcolor(int color);
    void set_bgcolor(int color);
    void set_rgbcolor(float r, float g, float b);
    void pos(int r, int c);
    std::string char_at(int row, int col);


    static const int rows = 25;
    static const int cols = 40;
    static const int width = 320;
    static const int height = 200;
    static const int b = 16; // Border

    int fgcolor = 14; // Text foreground
    int bgcolor = 6;  // Text background
    GLfloat color[4] = { 1.0, 1.0, 1.0, 1.0 };

    int row = 0;
    int col = 0;
    bool cursor = true;

    GLuint vertexArrayObjectID;
    GLuint framebufferID;
    GLuint textureID;
    GLuint shaderID;

  protected:


  private:
    void invert_cursor();
    void update_cursor();
    void enable_cursor();
    void disable_cursor();
    static double now();

    double cursor_interval = 0.5;
    double cursor_last = 0.0;

    // Draw all kinds of OpenGL primitives
    void draw_untextured_vbo(GLsizeiptr arrsize, const void* arr, GLenum type, GLsizei typesize, GLenum mode, GLsizei vertices);

    int clamp(int value, int minimum, int maximum);
    float clampf(float value, float minimum, float maximum);
    enum class Cursorstate {Disabled, Hidden, Normal, Inverse};
    Cursorstate cursor_state = Cursorstate::Normal;
    int cursor_hidden = 0; // hide_cursor increments; normal_cursor decrements; cursor hidden if > 0
    bool texture_enabled = false; // See set_fg_rgb()
    bool autoscroll_enabled = true;

    Fontcache font;
    Matrix4 m_ortho;

    GLint attr_vertex;
    GLint attr_uv;
    GLuint uniform_ortho;
    GLuint uniform_color;

    bool initialized = false;
    int ccmem[25][40];
    int fgmem[25][40];
    int bgmem[25][40];

    GLfloat rgb[16][3] = {
      {  0/255.0,   0/255.0,   0/255.0},  // 0  black
      {255/255.0, 255/255.0, 255/255.0},  // 1  white
      {146/255.0,  74/255.0,  74/255.0},  // 2  red
      {132/255.0, 197/255.0, 204/255.0},  // 3  cyan
      {147/255.0,  81/255.0, 182/255.0},  // 4  purple
      {114/255.0, 177/255.0,  75/255.0},  // 5  green
      { 72/255.0,  58/255.0, 170/255.0},  // 6  blue
      {213/255.0, 223/255.0, 124/255.0},  // 7  yellow
      {153/255.0, 105/255.0,  45/255.0},  // 8  lightbrown
      {103/255.0,  82/255.0,   0/255.0},  // 9  brown
      {193/255.0, 129/255.0, 120/255.0},  // 10 pink
      { 96/255.0,  96/255.0,  96/255.0},  // 11 darkgray
      {138/255.0, 138/255.0, 138/255.0},  // 12 gray
      {179/255.0, 236/255.0, 145/255.0},  // 13 lightgreen
      {134/255.0, 122/255.0, 222/255.0},  // 14 lightblue
      {179/255.0, 179/255.0, 179/255.0},  // 15 lightgray
    };
};

#endif // DISPLAY_H
