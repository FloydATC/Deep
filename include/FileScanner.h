#ifndef FILESCANNER_H
#define FILESCANNER_H

#include <string>

class FileScanner
{
  public:
    FileScanner();
    virtual ~FileScanner();

    bool load(std::string filename);
    void initialize();
    void advance();
    void consume(char c);
    void consume_eol();
    void skip_whitespace(bool incl_newline=false);

    bool is_eof();
    bool is_eol();
    bool is_alpha();
    bool is_numeric();
    bool is_delimiter();
    bool is_whitespace();
    bool is_dot();
    bool is_path_sep();

    std::string get_keyword();
    std::string get_filename();
    std::string get_string();
    float get_float();
    int get_integer();

    char what();
    std::string where();

  protected:

  private:

    int pos;
    int lineno;
    int charno;
    std::string filename;
    std::string source;

};

#endif // FILESCANNER_H
