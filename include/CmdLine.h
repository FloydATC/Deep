#ifndef CMDLINE_H
#define CMDLINE_H

#include <string>
#include <vector>

class CmdLine
{
  public:
    CmdLine();
    ~CmdLine();

    // Input
    void parse(std::string line);
    void parse(const char* line);

    // Output
    std::vector<std::string> args_vector();
    const char** args_array();
    int args_length();

  protected:

  private:
    std::string line;
    std::vector<std::string> args;
    std::vector<const char*> argv;
    int current;
    int start;
    bool is_escaped;
    bool escape_next;

    std::string unescape(std::string str);
    bool is_trailing_utf8();
    bool eol(int offset);
    void advance();
    void skip_whitespace();
    bool match(char c);
    char peek();
    std::string plain_string();
    std::string quoted_string();
    void argument();
    void parse_line();
};

#endif // CMDLINE_H
