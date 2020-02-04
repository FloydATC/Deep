#include <chrono>

#include "Display.h"
#include "Fontcache.h"
#include "utf8.h"
//#include <GL/glew.h>
//#include "SDL_opengl.h"
//#include "SDL.h"

Display::Display()
{
  //ctor
}

Display::Display(GLuint shaderID, Fontcache font)
{
  //ctor
  //this->framebufferID = framebufferID;
  //this->textureID = textureID;
  this->vertexArrayObjectID = prepare_vao();
  this->textureID = prepare_texture(width+b*2, height+b*2);
  this->framebufferID = prepare_framebuffer(textureID);
  this->shaderID = shaderID;
  this->font = font;
  //this->m_ortho = Matrix4().Ortho(-1.0f-b, 319.0f+b, -1.0f-b, 199.0f+b, -1.0f, 1.0f);
  this->m_ortho = Matrix4().Ortho(-0.5f, width-0.5f, -0.0f, height-0.0f, -1.0f, 1.0f);

  glUseProgram(shaderID);
  this->attr_vertex = glGetAttribLocation(shaderID, "vertex");
  this->attr_uv = glGetAttribLocation(shaderID, "uv");
  this->uniform_ortho = glGetUniformLocation(shaderID, "ortho");
  this->uniform_color = glGetUniformLocation(shaderID, "color");

}

Display::~Display()
{
  //dtor

}

// check OpenGL errors
void Display::check_gl(std::string when)
{
  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    std::cerr << "OpenGL error " << when << ": " << gluErrorString(err) << "(" << err << ")" << std::endl;
    //running = false;
  }
}


void Display::draw_untextured_vbo(GLsizeiptr arrsize, const void* arr, GLenum type, GLsizei typesize, GLenum mode, GLsizei vertices)
{
  GLuint vertexBufferID;
  glGenBuffers(1, &vertexBufferID);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
  glBufferData(GL_ARRAY_BUFFER, arrsize, arr, GL_STREAM_DRAW); // Buffer will be used only once
  glEnableVertexAttribArray(attr_vertex);
  glDisableVertexAttribArray(attr_uv);
  glVertexAttribPointer(attr_vertex, 2, type, GL_FALSE, 2*typesize, (char*) NULL+0*typesize);
  glDrawArrays(mode, 0, vertices);
  glDeleteBuffers(1, &vertexBufferID);
  glEnableVertexAttribArray(0);
}


void Display::draw_rect(int x, int y, int w, int h)
{
  int x1 = tx(x);
  int y1 = ty(y);
  int x2 = tx(x+w-1);
  int y2 = ty(y+h-1);
  // Draw a rectangle.
  GLint rect[] =
  {
       x1,     y1,
       x2,     y1,
       x2,     y2,
       x1,     y2,
  };
  //   x               y

  draw_untextured_vbo(sizeof(rect), rect, GL_INT, sizeof(GLint), GL_LINE_LOOP, 4);
}


void Display::draw_area(int x, int y, int w, int h)
{
  // Draw a filled rectangle.
  int x1 = tx(x);
  int y1 = ty(y);
  int x2 = tx(x+w-1);
  int y2 = ty(y+h-1);
  GLint rect[] =
  {
      x1,  y1, // upper left
      x2,  y1, // upper right
      x2,  y2, // lower right
      x2,  y2, // lower right
      x1,  y2, // lower left
      x1,  y1, // upper left
  };
  //   x               y

  draw_untextured_vbo(sizeof(rect), rect, GL_INT, sizeof(GLint), GL_TRIANGLES, 6);
}

void Display::draw_poly(int x1, int y1, int x2, int y2, int x3, int y3)
{
  x1 = tx(x1);
  y1 = ty(y1);
  x2 = tx(x2);
  y2 = ty(y2);
  x3 = tx(x3);
  y3 = ty(y3);
  // Draw a filled polygon
  GLint poly[] =
  {
     x1, y1, // v1
     x2, y2, // v2
     x3, y3, // v3
  };
  //   x               y

  draw_untextured_vbo(sizeof(poly), poly, GL_INT, sizeof(GLint), GL_TRIANGLES, 3);
}

void Display::draw_line(int x1, int y1, int x2, int y2)
{
  //std::cout << "Display::draw_line() x1=" << x1 << " y1=" << y1 << " x2=" << x2 << " y2=" << y2 << std::endl;
  x1 = tx(x1);
  y1 = ty(y1);
  x2 = tx(x2);
  y2 = ty(y2);
  // Draw a line.
  GLint line[] =
  {
    x1, y1,
    x2, y2,
  };
  // x            y

  draw_untextured_vbo(sizeof(line), line, GL_INT, sizeof(GLint), GL_LINES, 2);
}


void Display::draw_circle(int x, int y, int r)
{
  //std::cout << "Display::draw_circle() x=" << x << " y=" << y << " r=" << r << std::endl;
  // Draw a circle.
  int parts = 32;
  float two_pi = 2.0 * 3.1415;
  float theta;
  GLint points[parts*2] = { 0 };
  // These vertices describe the circle
  for (int i=0; i<parts; i++) {
    theta = (float(i)) / (float(parts)) * two_pi;
    //std::cout << "theta=" << theta << " ";
    points[i*2+0] = tx(x + (((float)r) * sin(theta)));
    points[i*2+1] = ty(y + (((float)r) * cos(theta)));
    //std::cout << "x=" << points[i*2+0] << " ";
    //std::cout << "y=" << points[i*2+1] << std::endl;
  }

  draw_untextured_vbo(sizeof(points), points, GL_INT, sizeof(GLint), GL_LINE_LOOP, parts);
}


void Display::draw_disc(int x, int y, int r)
{
  //std::cout << "Display::draw_circle() x=" << x << " y=" << y << " r=" << r << std::endl;
  // Draw a circle.
  int parts = 32;
  float two_pi = 2.0 * 3.1415;
  float theta;
  GLfloat points[(2+parts)*2] = { 0.0f };
  // The first vertex is the center
  points[0] = ((float)x);
  points[1] = height - ((float)y) - 1;
  // The remaining vertices describe the circle
  for (int i=0; i<parts+1; i++) {
    theta = (float(i)) / (float(parts)) * two_pi;
    //std::cout << "i=" << i << " ";
    //std::cout << "theta=" << theta << " ";
    points[2+i*2+0] = tx(x + (((float)r) * sin(theta)));
    points[2+i*2+1] = ty(y + (((float)r) * cos(theta)));
    //std::cout << "x=" << points[2+i*2+0] << " ";
    //std::cout << "y=" << points[2+i*2+1] << std::endl;
  }

  draw_untextured_vbo(sizeof(points), points, GL_FLOAT, sizeof(GLfloat), GL_TRIANGLE_FAN, parts+2);
}


// All of that for a single drop of blood
void Display::draw_point(int x, int y)
{
  //std::cout << "Display::draw_line() x1=" << x1 << " y1=" << y1 << " x2=" << x2 << " y2=" << y2 << std::endl;
  // Draw a line.
  GLint point[] =
  {
    tx(x), ty(y),
  };
  // x            y
  draw_untextured_vbo(sizeof(point), point, GL_INT, sizeof(GLint), GL_POINTS, 1);
}


void Display::print(const char* str)
{
  check_gl("print() enter");
  int len = strlen(str);
  //std::cout << "Display::print() str=" << str << " len=" << len << std::endl;
  if (len==0) return;
  int codepoint;
  //SDL_Surface* block = font.glyph(0x2588); // Use for drawing the background
  GLuint block = font.glyph(0x2588); // Use for drawing the background

  int i = 0;
  hide_cursor();
  while (i<len) {

    // Decode UTF8 into integer codepoint
    unsigned char u0 = str[i+0];
    if (u0>=0 && u0<=127) {
      i+=1; codepoint = u0; // Plain ASCII
    } else {
      if (len<2) return; // Invalid
      unsigned char u1 = str[i+1];
      if (u0>=192 && u0<=223) {
        i+=2; codepoint = (u0-192)*64 + (u1-128);
      } else {
        if (str[i+0]==0xed && (str[i+1] & 0xa0) == 0xa0) return; //code points, 0xd800 to 0xdfff
        if (len<3) return; // Invalid
        unsigned char u2 = str[i+2];
        if (u0>=224 && u0<=239) {
          i+=3; codepoint = (u0-224)*4096 + (u1-128)*64 + (u2-128);
        } else {
          if (len<4) return; // Invalid
          unsigned char u3 = str[i+3];
          if (u0>=240 && u0<=247) {
            i+=4; codepoint = (u0-240)*262144 + (u1-128)*4096 + (u2-128)*64 + (u3-128);
          }
        }
      }
    }

    // If printable, draw background then codepoint glyph
    if (codepoint == 10) {
      col=0; // \n
      row++;
    } else if (codepoint == 13) {
      col=0; // \r
    } else if (codepoint == 9) {
      col=(col/8)*8+8; // \t
    } else if (codepoint < 32) {
      // Unprintable
      //std::cerr << "Display::print() unprintable codepoint=" << codepoint << " ignored" << std::endl;
      col++;
    } else {
      ccmem[row][col] = codepoint;
      fgmem[row][col] = fgcolor;
      bgmem[row][col] = bgcolor;

      draw_surface(row, col, rgb[bgcolor][0], rgb[bgcolor][1], rgb[bgcolor][2], block);
      draw_surface(row, col, rgb[fgcolor][0], rgb[fgcolor][1], rgb[fgcolor][2], font.glyph(codepoint));
      col++;
    }
    if (col>=cols) { row++; col = 0; }
    if (row>=rows) {
      row=rows-1;
      if (autoscroll_enabled) { scroll_up(); }
    }
  }
  show_cursor();
}


void Display::draw_surface(int row, int col, float r, float g, float b, GLuint src_texture)
{
  check_gl("draw_surface(0) enter");
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendEquation(GL_FUNC_ADD);
  check_gl("draw_surface(0) blending setup");

  float x1 = tx(col*8);
  float y1 = ty((row+1)*8);
  float x2 = x1+8.0;
  float y2 = y1+8.0;

  // Glyph textures for 8x8px monospace bitmap font is (sometimes?) greater than 8x8px
  // Weirdness caused by the font or by SDL_ttf? Does it matter?
  // Handle it by mapping just a portion of the texture to our 8x8 pixels
  float u = 8.0 / font.width;
  float v = 8.0 / font.height;

  GLfloat rect[] =
  {
      x2, y2,    u, 0, // lower right
      x1, y2,  0.0, 0, // lower left
      x1, y1,  0.0, v,// upper left
      x1, y1,  0.0, v, // upper left
      x2, y1,    u, v, // upper right
      x2, y2,    u, 0, // lower right
  };
  //   x   y   u  v

  glUseProgram(shaderID);
  //check_gl("draw_surface(0)");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, src_texture);
  //check_gl("draw_surface(1)");
  GLfloat color[] = { r, g, b, 0.0 };
  glUniform4fv(uniform_color, 1, color);

  //check_gl("draw_surface(2)");
  GLuint vertexBufferID;
  glGenBuffers(1, &vertexBufferID);
  //check_gl("draw_surface(3)");
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
  //check_gl("draw_surface(4)");
  glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);
  //check_gl("draw_surface(5)");
  glEnableVertexAttribArray(attr_vertex);
  //check_gl("draw_surface(6)");
  glEnableVertexAttribArray(attr_uv);
  //check_gl("draw_surface(7)");
  glVertexAttribPointer(attr_vertex, 2, GL_FLOAT, GL_TRUE, 4*sizeof(GLfloat), (char*) NULL+0*sizeof(GLfloat));
  //check_gl("draw_surface(8)");
  glVertexAttribPointer(attr_uv, 2, GL_FLOAT, GL_TRUE, 4*sizeof(GLfloat), (char*) NULL+2*sizeof(GLfloat));
  //check_gl("draw_surface(9)");
  glDrawArrays(GL_TRIANGLES, 0, 6);
  //check_gl("draw_surface(10)");
  glDeleteBuffers(1, &vertexBufferID);
  //check_gl("draw_surface(11)");
  glEnableVertexAttribArray(0);
  //check_gl("draw_surface(12)");
  color[3] = 1.0;
  glUniform4fv(uniform_color, 1, color);
  check_gl("draw_surface(13)");

  //exit(99);
}


void Display::hide_cursor()
{
  //std::cout << "hiding cursor" << std::endl;
  if (cursor_hidden==0 && initialized==true) {
    int fgcolor = fgmem[row][col];
    int bgcolor = bgmem[row][col];
    draw_surface(row, col, rgb[bgcolor][0], rgb[bgcolor][1], rgb[bgcolor][2], font.glyph(0x2588));
    draw_surface(row, col, rgb[fgcolor][0], rgb[fgcolor][1], rgb[fgcolor][2], font.glyph(ccmem[row][col]));
    cursor_state = Cursorstate::Hidden;
  }
  //std::cout << "cursor hidden" << std::endl;
  cursor_hidden++;
}

void Display::show_cursor(bool force) {
  // Normally, calls to show_cursor() decrements a counter incremented by hide_cursor()
  // Only when the counter reaches zero the cursor is actually shown.
  // When a script terminates, the counter state is unknown and must be reset
  if (force) { cursor_hidden = 1; }
  show_cursor();
}

void Display::show_cursor()
{
  if (cursor_state == Cursorstate::Disabled) { return; }
  cursor_hidden--;
  //std::cout << "normal_cursor() cursor_hidden=" << cursor_hidden << std::endl;
  if (cursor_hidden > 0) { return; }
  if (cursor_hidden < 0) { cursor_hidden = 0; }
  int fgcolor = this->fgcolor; //fgmem[row][col];
  int bgcolor = bgmem[row][col];
  //std::cout << "normal_cursor() about to draw cursor" << std::endl;
  //std::cout << "row=" << row << " col=" << col << " ccmem=" << ccmem[row][col] << std::endl;
  draw_surface(row, col, rgb[fgcolor][0], rgb[fgcolor][1], rgb[fgcolor][2], font.glyph(0x2588));
  draw_surface(row, col, rgb[bgcolor][0], rgb[bgcolor][1], rgb[bgcolor][2], font.glyph(ccmem[row][col]));
  cursor_last = now();
  cursor_state = Cursorstate::Normal;
  //std::cout << "normal_cursor() done" << std::endl;
}

void Display::invert_cursor()
{
  if (cursor_state == Cursorstate::Disabled) { return; }
  int fgcolor = fgmem[row][col];
  int bgcolor = bgmem[row][col];
  draw_surface(row, col, rgb[bgcolor][0], rgb[bgcolor][1], rgb[bgcolor][2], font.glyph(0x2588));
  draw_surface(row, col, rgb[fgcolor][0], rgb[fgcolor][1], rgb[fgcolor][2], font.glyph(ccmem[row][col]));
  cursor_last = now();
  cursor_state = Cursorstate::Inverse;
}

void Display::update_cursor()
{
  if (cursor_state == Cursorstate::Disabled) { return; }
  if (cursor_state == Cursorstate::Hidden) { return; }
  if (now() > cursor_last + cursor_interval) {
    if (cursor_state == Cursorstate::Inverse) {
      show_cursor();
    } else {
      invert_cursor();
    }
  }
}

void Display::enable_cursor()
{
  cursor_state = Cursorstate::Hidden;
  show_cursor();
}

void Display::disable_cursor()
{
  hide_cursor();
  cursor_state = Cursorstate::Disabled;
}

void Display::set_rgbcolor(float r, float g, float b)
{
  float alpha = 1.0;
  if (texture_enabled) { alpha = 0.0; }
  // The fragment shader uses texture sampling (for font rendering etc) if alpha==0.0
  color[0] = clampf(r, 0.0, 1.0);
  color[1] = clampf(g, 0.0, 1.0);
  color[2] = clampf(b, 0.0, 1.0);
  color[3] = alpha;
  glUniform4fv(uniform_color, 1, color);
}

GLuint Display::prepare_vao()
{
  GLuint VertexArrayObjectID;
  glGenVertexArrays(1, &VertexArrayObjectID);
  glBindVertexArray(VertexArrayObjectID);
  return VertexArrayObjectID;
}

GLuint Display::prepare_texture(GLsizei width, GLsizei height)
{
  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  GLfloat bordercolor[4] = { 0.0, 0.0, 0.25, 1.0 };
  check_gl("binding framebuffer texture");
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0); // 0 = clear
  //glGenerateMipmap(GL_TEXTURE_2D);
  check_gl("creating framebuffer texture");
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bordercolor);
  check_gl("setting texture params");
  return textureID;
}

GLuint Display::prepare_framebuffer(GLuint textureID)
{
  // Create framebuffer
  GLuint framebufferID = 0;
  glGenFramebuffers(1, &framebufferID);
  glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
  check_gl("creating framebuffer");

  // Set "textureID" as our colour attachement #0
  glBindTexture(GL_TEXTURE_2D, textureID);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
  check_gl("attached framebuffer texture");

  // Set the list of draw buffers. (???)
  GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0}; // (???)
  glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers (???)
  check_gl("set drawbuffers (?)");

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "glCheckFramebufferStatus = " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
    //running = false;
  }
  return framebufferID;
}


bool Display::pre_render()
{
  glViewport(b,b,width,height); // b,b = LOWER LEFT corner. width,height is the actual drawing area.
  glBindVertexArray(vertexArrayObjectID);
  glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);

  glUseProgram(shaderID);
  //GLint attr_vertex = glGetAttribLocation(shaderID, "vertex");
  //GLuint uniform_ortho = glGetUniformLocation(shaderID, "ortho");
  //GLuint uniform_color = glGetUniformLocation(shaderID, "color");

  glUniformMatrix4fv(uniform_ortho, 1, GL_FALSE, m_ortho.get());
  glEnableVertexAttribArray(attr_vertex);

  //set_fg_rgb(rand()%256, rand()%256, rand()%256);
  //GLfloat color[] = { rand()%256/255.0f, rand()%256/255.0f, rand()%256/255.0f, 1.0 };
  //glUniform4fv(uniform_color, 1, color);

  //draw_rect(10,10,300,180);

/*
  glDrawArrays(GL_LINE_LOOP, 0, 4);
  glDeleteBuffers(1, &vertexBufferID);
*/
  if (initialized == false) {
    reset();
    //print((const char*) glGetString(GL_RENDERER));
    //print("\n");
    //print((const char*) glGetString(GL_VERSION));
    //print("\n");
  }

  return true;
}




bool Display::post_render()
{
  update_cursor();

  glDisableVertexAttribArray(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return true;
}



void Display::reset()
{
  fgcolor = 7;
  bgcolor = 11;
  cursor_hidden = 0;
  autoscroll_enabled = true;

  glClearColor(rgb[bgcolor][0], rgb[bgcolor][1], rgb[bgcolor][2], 1.0f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  cls();

  initialized = true;
}


std::string Display::char_at(int row, int col)
{
  if (row < 0) { row = 0; }
  if (col < 0) { col = 0; }
  if (row >= rows) { row = rows - 1; }
  if (col >= cols) { col = cols - 1; }
  char buf[5] = {0};
  char* err = nullptr;
  char* res = utf8::append(ccmem[row][col], buf, err);
  return res;
  //return Fontcache::UnicodeToUTF8(ccmem[row][col]);
}

void Display::cursor_up()
{
  hide_cursor();
  row--;
  if (row < 0) { row = 0; }
  show_cursor();
}

void Display::cursor_down()
{
  hide_cursor();
  row++;
  if (row >= rows) { row = rows-1; scroll_up(); }
  show_cursor();
}

void Display::cursor_left()
{
  hide_cursor();
  col--;
  if (col < 0) { col = 0; }
  show_cursor();
}

void Display::cursor_right()
{
  hide_cursor();
  col++;
  if (col >= cols) { col = cols-1; }
  show_cursor();
}

int Display::clamp(int value, int minimum, int maximum) {
  // Make sure value is >= minimum and <= maximum
  return (value < minimum ? minimum : ( value > maximum ? maximum : value ));
}

float Display::clampf(float value, float minimum, float maximum) {
  // Make sure value is >= minimum and <= maximum
  return (value < minimum ? minimum : ( value > maximum ? maximum : value ));
}

inline void Display::cls()
{
  row = 0;
  col = 0;
  cls(0, 0, rows-1, cols-1, 32);
}

inline void Display::cls(int r1, int c1, int r2, int c2)
{
  cls(r1, c1, r2, c2, 32);
}

void Display::cls(int r1, int c1, int r2, int c2, int codepoint)
{
  hide_cursor();
  //std::cout << "cls() c1="<< c1 <<" r1="<< r1 <<" c2="<< c2 <<" r2=" << r2 << std::endl;
  r1 = clamp(r1, 0, rows-1);
  c1 = clamp(c1, 0, cols-1);
  r2 = clamp(r2, 0, rows-1);
  c2 = clamp(c2, 0, cols-1);
  GLuint block = font.glyph(0x2588);
  GLuint surface = font.glyph(codepoint);
  for (int r=r1; r<=r2; r++) {
    for (int c=c1; c<=c2; c++) {
      ccmem[r][c] = codepoint;
      fgmem[r][c] = fgcolor;
      bgmem[r][c] = bgcolor;
      if (surface!=0) {
        draw_surface(r, c, rgb[bgcolor][0], rgb[bgcolor][1], rgb[bgcolor][2], block);
        draw_surface(r, c, rgb[fgcolor][0], rgb[fgcolor][1], rgb[fgcolor][2], surface);
      }
    }
  }
  show_cursor();
}

void Display::enable_autoscroll() {
  autoscroll_enabled = true;
}

void Display::disable_autoscroll() {
  autoscroll_enabled = false;
}

void Display::scroll_up()
{
  scroll_up(0, 0, rows-1, cols-1);
}

void Display::blit_text(int sc1, int sr1, int sc2, int sr2, int dc1, int dr1, int dc2, int dr2) {
  glBlitFramebuffer(
    tx((sc1+0)*8)+b, ty((sr1+0)*8)+b, tx((sc2+1)*8)+b, ty((sr2+1)*8)+b,
    tx((dc1+0)*8)+b, ty((dr1+0)*8)+b, tx((dc2+1)*8)+b, ty((dr2+1)*8)+b,
    GL_COLOR_BUFFER_BIT, GL_NEAREST
  );
}

void Display::scroll_up(int r1, int c1, int r2, int c2)
{
  hide_cursor();
  //std::cout << "Display() scroll_up" << std::endl;
  r1 = clamp(r1, 0, rows-1);
  c1 = clamp(c1, 0, cols-1);
  r2 = clamp(r2, 0, rows-1);
  c2 = clamp(c2, 0, cols-1);

  for (int r=r1; r<r2; r++) {
    // Copy ccmem, fgmem and bgmem
    //std::cout << "Processing row " << r << std::endl;
    for (int c=c1; c<=c2; c++) {
      ccmem[r][c] = ccmem[r+1][c];
      fgmem[r][c] = fgmem[r+1][c];
      bgmem[r][c] = bgmem[r+1][c];
    }
    // Blit texture
    blit_text( c1, r+1, c2, r+1,   c1, r+0, c2, r+0 );
  }
  //std::cout << "Display() scroll_up clearing bottom line" << std::endl;
  cls(r2, c1, r2, c2);
  //std::cout << "Display() scroll_up done" << std::endl;

  show_cursor();
}

void Display::scroll_down()
{
  scroll_down(0, 0, rows-1, cols-1);
}

void Display::scroll_down(int r1, int c1, int r2, int c2)
{
  r1 = clamp(r1, 0, rows-1);
  c1 = clamp(c1, 0, cols-1);
  r2 = clamp(r2, 0, rows-1);
  c2 = clamp(c2, 0, cols-1);
  hide_cursor();
  //std::cout << "Display() scroll_down" << std::endl;

  for (int r=r2-1; r>=r1; r--) {
    // Copy ccmem, fgmem and bgmem
    //std::cout << "Processing row " << r << std::endl;
    for (int c=c1; c<=c2; c++) {
      ccmem[r+1][c] = ccmem[r][c];
      fgmem[r+1][c] = fgmem[r][c];
      bgmem[r+1][c] = bgmem[r][c];
    }
    // Blit texture
    blit_text( c1, r+0, c2, r+0,   c1, r+1, c2, r+1 );
  }
  //std::cout << "Display() scroll_down clearing top line" << std::endl;
  cls(r1, c1, r1, c2);
  //std::cout << "Display() scroll_down done" << std::endl;

  show_cursor();
}

void Display::scroll_left()
{
  scroll_left(0, 0, rows-1, cols-1);
}

void Display::scroll_left(int r1, int c1, int r2, int c2)
{
  r1 = clamp(r1, 0, rows-1);
  c1 = clamp(c1, 0, cols-1);
  r2 = clamp(r2, 0, rows-1);
  c2 = clamp(c2, 0, cols-1);
  hide_cursor();
  //std::cout << "Display() scroll_left" << std::endl;

  for (int c=c1; c<c2; c++) {
    // Copy ccmem, fgmem and bgmem
    //std::cout << "Processing column " << c << std::endl;
    for (int r=r1; r<=r2; r++) {
      ccmem[r][c] = ccmem[r][c+1];
      fgmem[r][c] = fgmem[r][c+1];
      bgmem[r][c] = bgmem[r][c+1];
    }
    // Blit texture
    blit_text( c+1, r1, c+1, r2,   c+0, r1, c+0, r2 );
  }
  //std::cout << "Display() scroll_left clearing right column" << std::endl;
  cls(r1, c2, r2, c2);
  //std::cout << "Display() scroll_left done" << std::endl;

  show_cursor();
}

void Display::scroll_right()
{
  scroll_right(0, 0, rows-1, cols-1);
}

void Display::scroll_right(int r1, int c1, int r2, int c2)
{
  r1 = clamp(r1, 0, rows-1);
  c1 = clamp(c1, 0, cols-1);
  r2 = clamp(r2, 0, rows-1);
  c2 = clamp(c2, 0, cols-1);
  hide_cursor();
  //std::cout << "Display() scroll_right" << std::endl;

  for (int c=c2-1; c>=c1; c--) {
    // Copy ccmem, fgmem and bgmem
    //std::cout << "Processing column " << c << std::endl;
    for (int r=r1; r<=r2; r++) {
      ccmem[r][c+1] = ccmem[r][c];
      fgmem[r][c+1] = fgmem[r][c];
      bgmem[r][c+1] = bgmem[r][c];
    }
    // Blit texture
    blit_text( c+0, r1, c+0, r2,   c+1, r1, c+1, r2 );
  }
  //std::cout << "Display() scroll_right clearing left column" << std::endl;
  cls(r1, c1, r2, c1);
  //std::cout << "Display() scroll_right done" << std::endl;

  show_cursor();
}

void Display::set_fgcolor(int color)
{
  hide_cursor();
  fgcolor = color % 16;
  show_cursor();
}

void Display::set_bgcolor(int color)
{
  hide_cursor();
  bgcolor = color % 16;
  show_cursor();
}


void Display::pos(int r, int c)
{
  hide_cursor();
  r = clamp(r, 0, rows-1);
  c = clamp(c, 0, cols-1);
  row = r;
  col = c;
  show_cursor();
}


// STATIC
double Display::now() {
  auto time = std::chrono::system_clock::now().time_since_epoch();
  std::chrono::seconds seconds = std::chrono::duration_cast< std::chrono::seconds >(time);
  std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(time);
  return (double) seconds.count() + ((double) (ms.count() % 1000)/1000.0);
}
