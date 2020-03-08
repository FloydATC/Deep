#include <iostream>
#include "Message.h"



Message::Message(Type type)
{
  //ctor
  //std::cout << "Message created: " << this << std::endl;
  this->type = type;
}

Message::~Message()
{
  //dtor
  //std::cout << "Message destroyed: " << this << std::endl;
}


std::ostream& operator <<(std::ostream& stream, const Message* msg) {
  switch (msg->type) {
    case Message::Type::Break:
      stream << "<Message::Type::Break>"; break;
    case Message::Type::KeyDown:
      stream << "<Message::Type::KeyDown name=\"" << SDL_GetKeyName(msg->key.sym) << "\" scancode=" << msg->key.scancode << ">"; break;
    case Message::Type::KeyUp:
      stream << "<Message::Type::KeyDown name=\"" << SDL_GetKeyName(msg->key.sym) << "\" scancode=" << msg->key.scancode << ">"; break;
    case Message::Type::MouseButton:
      stream << "<Message::Type::MouseButton button=" << msg->button.number << " state=" << (msg->button.pressed ? "Down" : "Up") << ">"; break;
    case Message::Type::MouseMotion:
      stream << "<Message::Type::MouseMotion>"; break;
    case Message::Type::MouseWheel:
      stream << "<Message::Type::MouseWheel>"; break;
    case Message::Type::Quit:
      stream << "<Message::Type::Quit>"; break;
    case Message::Type::Resize:
      stream << "<Message::Type::Resize>"; break;
    case Message::Type::TextInput:
      stream << "<Message::Type::TextInput \"" << msg->text << "\">"; break;
    default:
      stream << "<Message::Type::UNKNOWN>"; break;
  }
  return stream;
}

