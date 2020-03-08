
#include "Dev/Mouse.h"

#include <iostream>

#define MAX_CLICKTIME_MS 100


Mouse::Mouse()
{
  //ctor
  reset();
#ifdef DEBUG_TRACE_MOUSE
  std::cout << "Mouse" << this << " created" << std::endl;
#endif
}

Mouse::~Mouse()
{
  //dtor
  reset();
#ifdef DEBUG_TRACE_MOUSE
  std::cout << "Mouse" << this << " destroyed" << std::endl;
#endif
}





void Mouse::motion(Vector2 position)
{
  this->relative = position - this->absolute;
  this->absolute = position;

  // Create move event
  MouseEvent* event = new MouseEvent(MouseEvent::Type::Move);
  event->absolute = this->absolute;
  event->relative = this->relative;
  add_event( event );

  // Check for held down buttons, create drag events for each
  for (auto& button : buttons) {
    if (button.second->is_held()) {
      add_event( button.second->drag(position) );
    }
  }
}


void Mouse::button_down(int number, Vector2 position)
{
  MouseButton* button = get_button(number);
  add_event( button->down(position) );
}


void Mouse::button_up(int number, Vector2 position)
{
  MouseButton* button = get_button(number);
  add_event( button->up(position) );
}


void Mouse::check()
{
  // Check each button timer to see if an event is needed
  // e.g. if a button has been held down long enough to drag
  for (auto& button : buttons) add_event( button.second->check() );
}

Vector2 Mouse::get_absolute()
{
  return this->absolute;
}


Vector2 Mouse::get_relative()
{
  Vector2 result = this->relative;
  this->relative = Vector2(0.0, 0.0); // Reset on read
  return result;
}


MouseEvent* Mouse::get_event()
{
  this->active = true;
  if (events.empty()) return nullptr;
  MouseEvent* event = events.front();
  events.pop();
  return event;
}


void Mouse::add_event(MouseEvent* event)
{
  if (event == nullptr) return;
  //if (this->active == false) return; // Do not collect events unless a script wants them
  this->events.push(event);
#ifdef DEBUG_TRACE_MOUSE
  std::cout << "Mouse" << this << "::add_events() " << event << std::endl;
#endif
}


MouseButton* Mouse::get_button(int number)
{
  // Create the mousebutton object on demand
  if (buttons.count(number) == 0) buttons.insert({number, new MouseButton(number)});
  return buttons.at(number);
}


void Mouse::reset()
{
  this->active = false;
  this->absolute = Vector2(0.0, 0.0);
  this->relative = Vector2(0.0, 0.0);
  reset_buttons();
  reset_events();
#ifdef DEBUG_TRACE_MOUSE
  std::cout << "Mouse" << this << "::initialize() ready" << std::endl;
#endif
}

void Mouse::reset_buttons()
{
  for (auto& button : buttons) delete button.second;
  buttons.clear();
}


void Mouse::reset_events()
{
  while (!events.empty()) { delete events.front(); events.pop(); }
  events = std::queue<MouseEvent*>(); // std::queue has no .clear()
}

