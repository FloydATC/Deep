#ifndef UTF8_H
#define UTF8_H

#include <vector>

class UTF8hack
{
  public:
    //UTF8();
    //~UTF8();

    static char* append(int cp, char* result, char* error);
    static std::vector<int> DEPRECATED_codepoints(const char* str);
    static std::vector<uint32_t> codepoints(const char* str, int strlen);
  protected:

  private:
    static int get_codepoint(const char* str, int offset, int* bytes);
};

#endif // UTF8_H
