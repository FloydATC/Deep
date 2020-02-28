#ifndef POLYGON2D_H
#define POLYGON2D_H

#include "2D/Shape2D.h"

class Polygon2D : public Shape2D
{
  public:
    Polygon2D();
    ~Polygon2D();

    void draw(int x1, int y1, int x2, int y2, int x3, int y3);

  protected:

  private:
};

#endif // POLYGON2D_H

