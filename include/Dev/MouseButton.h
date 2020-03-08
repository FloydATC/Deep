#ifndef MOUSEBUTTON_H
#define MOUSEBUTTON_H

#include "DateTime.h"
#include "Dev/MouseEvent.h"
#include "Vectors.h"


class MouseButton
{
  public:
    MouseButton(int number);
    virtual ~MouseButton();


    MouseEvent* down(Vector2 position);
    MouseEvent* up(Vector2 position);
    MouseEvent* check();
    MouseEvent* drag(Vector2 position);
    bool is_held();

  protected:

  private:
    int number;
    bool is_down;
    bool is_waiting;
    int clicks;
    Vector2 position;
    Time when;
};

#endif // MOUSEBUTTON_H
