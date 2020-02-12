#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <queue>

#include <SDL.h>
#include "GameState.h"
#include "Message.h"

/*

  This class handles SDL events, translates relevant ones into
  internal Message objects and places them in a queue

  OS -> SDL_Event -> Message -> message_queue

*/



class EventHandler
{
  public:
    EventHandler();
    ~EventHandler();

    void handle_event(SDL_Event event);
    bool has_message();
    Message* get_message();

  protected:

  private:
    void send_message(Message* msg);

    void handle_window_size_changed(SDL_Event event);
    void handle_textinput(SDL_Event event);
    void handle_keydown(SDL_Event event);
    void handle_keyup(SDL_Event event);
    void handle_mousemotion(SDL_Event event);
    void handle_mousebuttondown(SDL_Event event);
    void handle_mousebuttonup(SDL_Event event);
    void handle_mousewheel(SDL_Event event);
    void handle_quit(SDL_Event event);
    void handle_windowevent(SDL_Event event);

    std::queue<Message*> message_queue;

};

#endif // EVENTHANDLER_H
