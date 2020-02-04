#ifndef UTF8_H
#define UTF8_H

#include <vector>

class utf8
{
  public:
    //utf8();
    //~utf8();

    static char* append(int cp, char* result, char* error);
    static std::vector<int> codepoints(const char* str);
  protected:

  private:
};

#endif // UTF8_H
