#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

#include <SDL.h> // For SDL_Key enumerations

class Message
{
  public:

    enum class Type {
      Quit             = 0x00,
      Resize           = 0x01,
      TextInput        = 0x02,
      Break            = 0x03, // Ctrl+C
      MouseMotion      = 0x10,
      MouseButton      = 0x11,
      MouseWheel       = 0x12,
      KeyDown          = 0x21,
      KeyUp            = 0x22,
    };

    struct MouseButton {
      int x;
      int y;
      int number;
      bool pressed;
    };

    struct MouseMotion {
      int x;
      int y;
      int xrel;
      int yrel;
    };

    struct MouseWheel {
      int x;
      int y;
    };

    struct Screen {
      int width;
      int height;
    };

    struct Key {
      Uint16 mod;
      SDL_Scancode scancode;
      SDL_Keycode sym;
    };

    //Message();
    Message(Type type);
    ~Message();



    Type type;
    struct MouseButton button;
    struct MouseMotion motion;
    struct MouseWheel wheel;
    struct Screen screen;
    struct Key key;
    std::string text;

    /* Y U no work?!
    std::ostream& operator<< (std::ostream& stream); //, const Message& msg);
    */

  protected:

  private:
};

std::ostream& operator <<(std::ostream& stream, const Message* msg);

#endif // MESSAGE_H
