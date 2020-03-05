#include "CmdLine.h"

#include <iostream>


#define DEBUG_TRACE_CMDLINE

CmdLine::CmdLine()
{
  //ctor
}

CmdLine::~CmdLine()
{
  //dtor
}


// User input methods

void CmdLine::parse(std::string line)
{
  this->line = line;
  parse_line();
}


void CmdLine::parse(const char* line)
{
  this->line = std::string(line);
  parse_line();
}


// User output methods

std::vector<std::string> CmdLine::args_vector()
{
  return this->args;
}


const char** CmdLine::args_array()
{
  std::vector<const char*> char_array;
  for (int i=0; i<(int)this->args.size(); i++) {
    char_array.push_back(this->args[i].c_str());
  }
  return char_array.data();
}


int CmdLine::args_length()
{
  return this->args.size();
}




// Internal parse methods


std::string CmdLine::unescape(std::string str)
{
  std::string result = "";
  bool take_next = false;
  for (int i=0; i<(int)str.size(); i++) {
    if (take_next == true) {
      result.append(str.substr(i, 1));
      take_next = false;
      continue;
    }
    if (str[i] == '\\') {
      take_next = true;
      continue;
    }
    result.append(str.substr(i, 1));
  }
#ifdef DEBUG_TRACE_CMDLINE
  std::cout << "CmdLine::unescape() '" << str << "' => '" << result << "'" << std::endl;
#endif
  return result;
}


bool CmdLine::eol(int offset)
{
  return this->current + offset >= (int)this->line.size();
}


void CmdLine::advance()
{
  if (eol(0)) return; // Can't advance beyond end of line
  this->current++;
#ifdef DEBUG_TRACE_CMDLINE
  std::cout << "CmdLine::advance() current=" << this->current << " char=" << this->line[this->current] << std::endl;
#endif
  if (this->escape_next == true) {
    // The previous character was the start of an escape sequence
    this->escape_next = false;
    this->is_escaped = true;
    return;
  }
  this->is_escaped = false;
  if (match('\\')) this->escape_next = true;
}



void CmdLine::skip_whitespace()
{
#ifdef DEBUG_TRACE_CMDLINE
  std::cout << "CmdLine::skip_whitespace() begin start=" << this->start << " current=" << this->current << std::endl;
#endif
  while (match(' ')) advance();
  this->start = this->current;
#ifdef DEBUG_TRACE_CMDLINE
  std::cout << "CmdLine::skip_whitespace() done start=" << this->start << " current=" << this->current << std::endl;
#endif
}


bool CmdLine::match(char c)
{
  return (this->line[this->current] == c);
}

char CmdLine::peek()
{
  if (eol(1)) return '\0';
  return this->line[this->current + 1];
}

std::string CmdLine::plain_string()
{
#ifdef DEBUG_TRACE_CMDLINE
  std::cout << "CmdLine::plain_string() begin start=" << this->start << " current=" << this->current << std::endl;
#endif
  while (!eol(0) && (this->is_escaped || !match(' '))) advance();
  std::string result = this->line.substr(this->start, this->current - this->start);
  this->start = this->current;
#ifdef DEBUG_TRACE_CMDLINE
  std::cout << "CmdLine::plain_string() done start=" << this->start << " current=" << this->current << std::endl;
#endif
  return unescape(result);
}

std::string CmdLine::quoted_string()
{
  advance(); // consume leading quote
  while (!eol(0) && (this->is_escaped || !match('"'))) advance();
  advance(); // Consume ending quote
  std::string result = this->line.substr(this->start+1, this->current - this->start - 2); // Drop quotes
  this->start = this->current;
  return unescape(result);
}


void CmdLine::argument()
{
#ifdef DEBUG_TRACE_CMDLINE
  std::cout << "CmdLine::argument() begin" << std::endl;
#endif
  skip_whitespace();
  if (eol(0)) return;
  if (match('"')) {
    this->args.push_back(quoted_string());
  } else {
    this->args.push_back(plain_string());
  }
#ifdef DEBUG_TRACE_CMDLINE
  std::cout << "CmdLine::argument() done" << std::endl;
#endif
}

void CmdLine::parse_line()
{
#ifdef DEBUG_TRACE_CMDLINE
  std::cout << "CmdLine::parse_line() begin" << std::endl;
#endif
  this->start = 0;
  this->current = -1;
  this->is_escaped = false;
  this->escape_next = false;
  advance();
  while (!eol(0)) argument();
#ifdef DEBUG_TRACE_CMDLINE
  std::cout << "CmdLine::parse_line() done" << std::endl;
#endif
}


