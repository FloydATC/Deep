#include "Polygon2D.h"

Polygon2D::Polygon2D()
{
  //ctor
}

Polygon2D::~Polygon2D()
{
  //dtor
}


void Polygon2D::draw(int x1, int y1, int x2, int y2, int x3, int y3)
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
