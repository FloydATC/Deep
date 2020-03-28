#include "FileScanner.h"

#include <iostream>

#include "IO/IOFile.h"


//#define DEBUG_TRACE_SCANNER

FileScanner::FileScanner()
{
  //ctor
#ifdef DEBUG_TRACE_SCANNER
  std::cout << "FileScanner" << this << " created" << std::endl;
#endif
}

FileScanner::~FileScanner()
{
  //dtor
#ifdef DEBUG_TRACE_SCANNER
  std::cout << "FileScanner" << this << " destroyed" << std::endl;
#endif
}


bool FileScanner::load(std::string filename)
{
#ifdef DEBUG_TRACE_SCANNER
  std::cout << "FileScanner" << this << "::load() filename=" << filename << std::endl;
#endif
  IOFile file;
  this->source = file.slurp(filename);
  if (this->source.size() == 0) return false;
  this->filename = filename;
  initialize();
  return true;
}

void FileScanner::initialize()
{
  lineno = 1;
  charno = 0;
  pos = -1;
  advance();
}

char FileScanner::what()
{
  return source[pos];
}

std::string FileScanner::where()
{
  return this->filename + " line " + std::to_string(lineno) + " char " + std::to_string(charno);
}

void FileScanner::advance()
{
  if (is_eof()) return;
  pos++;
  charno++;
  if (source[pos] == '\n') { lineno++; charno=1; }
}

void FileScanner::consume(char c)
{
  if (is_eof()) return;
  if (source[pos] != c) {
    std::cerr << "FileScanner: expected character code " << (int)c << ", found " << (int)what() << " in " << where() << std::endl;
  }
  advance();
}

void FileScanner::consume_eol()
{
  if (!is_eol()) {
    std::cerr << "FileScanner: expected EOL, found " << (int)what() << " in " << where() << std::endl;
  }
  while (is_eol()) advance();
}

void FileScanner::skip_whitespace(bool incl_newline)
{
#ifdef DEBUG_TRACE_SCANNER
  std::cout << "FileScanner" << this << "::skip_whitespace() " << where() << std::endl;
#endif
  while (is_whitespace() || (incl_newline && is_eol())) {
    if (source[pos] == '#') {
#ifdef DEBUG_TRACE_SCANNER
      std::cout << "FileScanner" << this << "::skip_whitespace() found comment" << std::endl;
#endif
      // Comment, scan until newline character is found
      while (!is_eof() && !is_eol()) { advance(); }
      advance(); // Consume the newline character as well
      continue;
    }
    advance();
  }
}

std::string FileScanner::get_keyword()
{
#ifdef DEBUG_TRACE_SCANNER
  std::cout << "FileScanner" << this << "::get_keyword() " << where() << std::endl;
#endif
  std::string keyword = "";
  while (is_eol()) advance();
  while (is_alpha()) {
    keyword += source[pos];
    advance();
  }
  skip_whitespace();
  return keyword;
}

std::string FileScanner::get_filename()
{
#ifdef DEBUG_TRACE_SCANNER
  std::cout << "FileScanner" << this << "::get_filename() " << where() << std::endl;
#endif
  std::string keyword = "";
  while (is_eol()) advance();
  while (is_alpha() || is_dot() || is_path_sep()) {
    keyword += source[pos];
    advance();
  }
  skip_whitespace();
  return keyword;
}

std::string FileScanner::get_string()
{
#ifdef DEBUG_TRACE_SCANNER
  std::cout << "FileScanner" << this << "::get_string() " << where() << std::endl;
#endif
  std::string keyword = "";
  while (is_eol()) advance();
  while (!is_whitespace() && !is_eol()) {
    keyword += source[pos];
    advance();
  }
  skip_whitespace();
  return keyword;
}

float FileScanner::get_float()
{
#ifdef DEBUG_TRACE_SCANNER
  std::cout << "FileScanner" << this << "::get_float() " << where() << std::endl;
#endif
  size_t length;
  float n = std::stof(source.substr(pos), &length);
  pos += (int)length;
  skip_whitespace();
  return n;
}

int FileScanner::get_integer()
{
#ifdef DEBUG_TRACE_SCANNER
  std::cout << "FileScanner" << this << "::get_integer() " << where() << std::endl;
#endif
  size_t length;
  //std::cout << "FileScanner::get_integer() lineno=" << lineno << " str=" << source.substr(pos,5) << std::endl;
  int n = std::stoi(source.substr(pos), &length, 10);
  pos += (int)length;
  skip_whitespace();
  return n;
}

bool FileScanner::is_eof()
{
  return (pos == (int) source.size());
}

bool FileScanner::is_eol()
{
  if (is_eof()) return false;
  return (source[pos] == '\r' || source[pos] == '\n');
}

bool FileScanner::is_alpha()
{
  if (is_eof()) return false;
  return (source[pos] >= 'a' && source[pos] <= 'z')
      || (source[pos] >= 'A' && source[pos] <= 'Z')
      || (source[pos] == '_');
}

bool FileScanner::is_numeric()
{
  if (is_eof()) return false;
  return (source[pos] >= '0' && source[pos] <= '9')
      || (source[pos] == '-');
}

bool FileScanner::is_delimiter()
{
  if (is_eof()) return false;
  return (source[pos] == '/' || source[pos] == ',');
}

bool FileScanner::is_whitespace()
{
  if (is_eof()) return false;
  return (source[pos] == '#' || source[pos] == ' ' || source[pos] == '\r');
}

bool FileScanner::is_dot()
{
  if (is_eof()) return false;
  return (source[pos] == '.');
}

bool FileScanner::is_path_sep()
{
  if (is_eof()) return false;
  return (source[pos] == '/' || source[pos] == '\\');
}

