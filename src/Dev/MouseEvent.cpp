
#include "Dev/MouseEvent.h"
#include "Dev/Mouse.h"

MouseEvent::MouseEvent(MouseEvent::Type type)
{
  //ctor
  this->type = type;
  this->button = 0;
  this->clicks = 0;
  this->absolute = Vector2(0.0, 0.0);
  this->relative = Vector2(0.0, 0.0);
#ifdef DEBUG_TRACE_MOUSE
  std::cout << this << " created" << std::endl;
#endif
}

MouseEvent::~MouseEvent()
{
  //dtor
#ifdef DEBUG_TRACE_MOUSE
  std::cout << this << " destroyed" << std::endl;
#endif
}


std::string MouseEvent::type_as_string()
{
  switch (this->type) {
    case MouseEvent::Type::Move: return "move";
    case MouseEvent::Type::Down: return "down";
    case MouseEvent::Type::Hold: return "hold";
    case MouseEvent::Type::Click: return "click";
    case MouseEvent::Type::Drag: return "drag";
    case MouseEvent::Type::Up: return "up";
    default: return "unknown";
  }
}


std::ostream& operator <<(std::ostream& stream, const MouseEvent* event) {
  switch (event->type) {
    case MouseEvent::Type::Move:
      stream << "<MouseEvent" << &event << "::Type::Move relative=" << event->relative << ">"; break;
    case MouseEvent::Type::Down:
      stream << "<MouseEvent" << &event << "::Type::Down button=" << event->button << ">"; break;
    case MouseEvent::Type::Hold:
      stream << "<MouseEvent" << &event << "::Type::Hold button=" << event->button << ">"; break;
    case MouseEvent::Type::Drag:
      stream << "<MouseEvent" << &event << "::Type::Drag button=" << event->button << " relative=" << event->relative << ">"; break;
    case MouseEvent::Type::Up:
      stream << "<MouseEvent" << &event << "::Type::Up button=" << event->button << ">"; break;
    case MouseEvent::Type::Click:
      stream << "<MouseEvent" << &event << "::Type::Click button=" << event->button << " clicks=" << event->clicks << ">"; break;
    default:
      stream << "<MouseEvent" << &event << "::Type::UNKNOWN>"; break;
  }
  return stream;
}

