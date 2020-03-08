#ifndef MOUSE_H
#define MOUSE_H


#include "Vectors.h"

#include <queue>
#include <unordered_map>

#include "Dev/MouseButton.h"
#include "Dev/MouseEvent.h"
#include "Time.h"


//#define DEBUG_TRACE_MOUSE


class Mouse
{
  public:
    Mouse();
    ~Mouse();

    void reset();

    // Input methods
    void motion(Vector2 position);
    void button_down(int number, Vector2 position);
    void button_up(int number, Vector2 position);
    void check();

    // Readout methods
    MouseEvent* get_event();
    Vector2 get_absolute();
    Vector2 get_relative();

  protected:

  private:
    bool active;
    Vector2 absolute;
    Vector2 relative;
    std::unordered_map<int, MouseButton*> buttons;
    std::queue<MouseEvent*> events;

    void add_event(MouseEvent* event);
    MouseButton* get_button(int number);


    void reset_buttons();
    void reset_events();

};

#endif // MOUSE_H
