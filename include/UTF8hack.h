#ifndef UTF8_H
#define UTF8_H

#include <vector>

class UTF8hack
{
  public:
    //UTF8();
    //~UTF8();

    static int u8_strlen(char *s);
    static char* append(int cp, char* result, char* error);
    static std::vector<uint32_t> codepoints(const char* str, int strlen);
  protected:

  private:
};

#endif // UTF8_H
