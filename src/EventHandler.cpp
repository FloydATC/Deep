#include "EventHandler.h"

#include <iostream>

EventHandler::EventHandler()
{
  //ctor
}

EventHandler::~EventHandler()
{
  //dtor

  // Flush queue
  while (!message_queue.empty())
  {
    delete message_queue.front();
    message_queue.pop();
  }
}


void EventHandler::handle_window_size_changed(SDL_Event event)
{
  Message* msg = new Message(Message::Type::Resize);
  msg->screen.width = event.window.data1;
  msg->screen.height = event.window.data2;
  send_message(msg);
}

void EventHandler::handle_textinput(SDL_Event event)
{
  Message* msg = new Message(Message::Type::TextInput);
  msg->text = event.text.text;
  send_message(msg);
}

void EventHandler::handle_keydown(SDL_Event event)
{
  Message* msg = new Message(Message::Type::KeyDown);
  // https://wiki.libsdl.org/SDL_Keysym
  msg->key.mod      = event.key.keysym.mod;      // SDL_Keymod (alt, shift, ctrl etc.)
  msg->key.scancode = event.key.keysym.scancode; // SDL_Scancode (physical)
  msg->key.sym      = event.key.keysym.sym;      // SDL_Keycode (logical)
  send_message(msg);
}

void EventHandler::handle_keyup(SDL_Event event)
{
  Message* msg = new Message(Message::Type::KeyUp);
  send_message(msg);
}

void EventHandler::handle_mousemotion(SDL_Event event)
{
  Message* msg = new Message(Message::Type::MouseMotion);
  msg->motion.x = event.motion.x;
  msg->motion.y = event.motion.y;
  msg->motion.xrel = event.motion.xrel;
  msg->motion.yrel = event.motion.yrel;
  send_message(msg);
}

void EventHandler::handle_mousebuttondown(SDL_Event event)
{
  Message* msg = new Message(Message::Type::MouseButton);
  msg->button.number = event.button.button;
  msg->button.pressed = true;
  msg->button.x = event.button.x;
  msg->button.y = event.button.y;
  send_message(msg);
}

void EventHandler::handle_mousebuttonup(SDL_Event event)
{
  Message* msg = new Message(Message::Type::MouseButton);
  msg->button.number = event.button.button;
  msg->button.pressed = false;
  msg->button.x = event.button.x;
  msg->button.y = event.button.y;
  send_message(msg);
}

void EventHandler::handle_mousewheel(SDL_Event event)
{
  Message* msg = new Message(Message::Type::MouseWheel);
  msg->wheel.x = event.wheel.x;
  msg->wheel.y = event.wheel.y;
  send_message(msg);
}

void EventHandler::handle_quit(SDL_Event event)
{
  Message* msg = new Message(Message::Type::Quit);
  send_message(msg);
}

void EventHandler::handle_windowevent(SDL_Event event)
{
  switch (event.window.event)
  {
    case SDL_WINDOWEVENT_SHOWN: break;
    case SDL_WINDOWEVENT_HIDDEN: break;
    case SDL_WINDOWEVENT_EXPOSED: break;
    case SDL_WINDOWEVENT_RESIZED: break;
    case SDL_WINDOWEVENT_SIZE_CHANGED: handle_window_size_changed(event); break;
    case SDL_WINDOWEVENT_MOVED: break;
    case SDL_WINDOWEVENT_MINIMIZED: break;
    case SDL_WINDOWEVENT_MAXIMIZED: break;
    case SDL_WINDOWEVENT_RESTORED:  break;
    case SDL_WINDOWEVENT_ENTER:  break;
    case SDL_WINDOWEVENT_LEAVE: break;
    case SDL_WINDOWEVENT_FOCUS_GAINED: break;
    case SDL_WINDOWEVENT_FOCUS_LOST: break;
    case SDL_WINDOWEVENT_CLOSE: break;
    default:
      std::cout << "main() unhandled WINDOW event=" << event.window.event << std::endl;
      break;
  }
}

void EventHandler::handle_event(SDL_Event event)
{
  switch (event.type)
  {
    case SDL_QUIT:                  handle_quit(event); break;
    case SDL_WINDOWEVENT:           handle_windowevent(event); break;
    case SDL_TEXTINPUT:             handle_textinput(event); break;
    case SDL_TEXTEDITING:           break;
    case SDL_MOUSEMOTION:           handle_mousemotion(event); break;
    case SDL_MOUSEBUTTONDOWN:       handle_mousebuttondown(event); break;
    case SDL_MOUSEBUTTONUP:         handle_mousebuttonup(event); break;
    case SDL_MOUSEWHEEL:            handle_mousewheel(event); break;
    case SDL_KEYDOWN:               handle_keydown(event); break;
    case SDL_KEYUP:                 handle_keyup(event); break;
    case SDL_SYSWMEVENT:            break;
    case SDL_AUDIODEVICEADDED:      break;
    case SDL_AUDIODEVICEREMOVED:    break;
    default:
      std::cout << "main() unhandled SDL_Event type=" << event.type << std::endl;
  }
}


void EventHandler::send_message(Message* msg)
{
  // Send message to GameThread (if any)
  if (msg != nullptr) {
    //std::unique_lock<std::mutex> lk(m_mutex);
    //std::cout << "main() dispatching message " << msg << std::endl;
    this->message_queue.push(msg);
  }
}

bool EventHandler::has_message()
{
  return !message_queue.empty();
}

Message* EventHandler::get_message()
{
  if (message_queue.empty()) return nullptr;
  Message* msg = message_queue.front();
  message_queue.pop();
  return msg;
}

