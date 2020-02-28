#ifndef OBJ3DSCANNER_H
#define OBJ3DSCANNER_H

#include <string>

class Obj3DScanner
{
  public:
    Obj3DScanner();
    ~Obj3DScanner();

    void load(std::string filename);
    void initialize();
    void advance();
    void consume(char c);
    void skip_whitespace();

    bool is_eof();
    bool is_eol();
    bool is_alpha();
    bool is_numeric();
    bool is_delimiter();
    bool is_whitespace();

    std::string get_keyword();
    std::string get_string();
    float get_float();
    int get_integer();

  protected:

  private:

    int pos;
    int lineno;
    std::string filename;
    std::string source;
};

#endif // OBJ3DSCANNER_H
