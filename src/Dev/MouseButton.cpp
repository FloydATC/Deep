
#include "Dev/MouseButton.h"
#include "Dev/Mouse.h"

#include <iostream>


#define MOUSE_TIME_MS 200

MouseButton::MouseButton(int number)
{
  //ctor
  this->number         = number;
  this->is_down        = false;
  this->is_waiting     = false;
  this->clicks         = 0;
  this->position       = Vector2(0.0, 0.0);
  this->when           = time_now();
#ifdef DEBUG_TRACE_MOUSE
  std::cout << "MouseButton" << this << " created" << std::endl;
#endif
}

MouseButton::~MouseButton()
{
  //dtor
#ifdef DEBUG_TRACE_MOUSE
  std::cout << "MouseButton" << this << " destroyed" << std::endl;
#endif
}



MouseEvent* MouseButton::up(Vector2 position)
{
  if (this->is_down == false) return nullptr; // Already up, do nothing (confused)
  Time t = time_now();

  MouseEvent* event = new MouseEvent( MouseEvent::Type::Up );
  event->button = this->number;
  event->absolute = position;
  event->relative = position - this->position;

  // Register as click if fast enough but do not generate event yet
  // because this may be part of a double-click
  double elapsed = time_elapsed_ms(t, this->when);
  if (elapsed < MOUSE_TIME_MS) this->clicks++;

  this->position = position;
  this->is_down = false;
  this->when = t;
  this->is_waiting = true;
  return event;
}


MouseEvent* MouseButton::down(Vector2 position)
{
  MouseEvents events;
  if (this->is_down == true) return nullptr; // Already down, do nothing (confused)
  Time t = time_now();

  MouseEvent* event = new MouseEvent( MouseEvent::Type::Down );
  event->button = this->number;
  event->absolute = position;

  this->position = position;
  this->is_down = true;
  this->when = t;
  this->is_waiting = true;
  return event;
}


bool MouseButton::is_held()
{
  return this->is_down;
}


MouseEvent* MouseButton::drag(Vector2 position)
{
  // Button is being held down and mouse is moving so generate Drag event
  MouseEvent* event = new MouseEvent( MouseEvent::Type::Drag );
  event->button = this->number;
  event->absolute = position;
  event->relative = position - this->position; // Relative to where the mouse was pressed down
  return event;
}


MouseEvent* MouseButton::check()
{
  if (this->is_waiting == false) return nullptr;

  if (time_elapsed_ms(time_now(), this->when) >= MOUSE_TIME_MS) {
    MouseEvent* event = nullptr;
    if (this->is_down) {
      // Button is being held down so generate Hold event
      event = new MouseEvent( MouseEvent::Type::Hold );
      event->button = this->number;
      event->absolute = this->position;
    } else {
      // Button has been released after a click so generate Click event
      event = new MouseEvent( MouseEvent::Type::Click );
      event->clicks = this->clicks;
      event->button = this->number;
      event->absolute = this->position;
      this->clicks = 0; // Reset counter
    }
    this->is_waiting = false;
    return event;
  }
  return nullptr;
}

