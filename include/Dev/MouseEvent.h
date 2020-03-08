#ifndef MOUSEEVENT_H
#define MOUSEEVENT_H

#include <iostream>
#include <vector>
#include <string>

#include "Vectors.h"

class MouseEvent
{
  public:

    enum class Type {
      Move             = 0x00,
      Down             = 0x01,
      Hold             = 0x02,
      Drag             = 0x03,
      Up               = 0x04,
      Click            = 0x05,
    };

    MouseEvent(MouseEvent::Type type);
    ~MouseEvent();

    MouseEvent::Type type;
    int button;
    int clicks;
    Vector2 absolute;
    Vector2 relative;

    std::string type_as_string();

  protected:

  private:
};

typedef std::vector<MouseEvent*> MouseEvents;
std::ostream& operator <<(std::ostream& stream, const MouseEvent* event);



#endif // MOUSEEVENT_H
