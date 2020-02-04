#include "Obj3DScanner.h"

#include <iostream>

#include "IOFile.h"

Obj3DScanner::Obj3DScanner()
{
  //ctor
}

Obj3DScanner::~Obj3DScanner()
{
  //dtor
}


void Obj3DScanner::load(std::string filename)
{
  //IOFile file = IOFile(filename);
  IOFile file;
  this->source = file.slurp(filename);
  this->filename = filename;
  initialize();
}

void Obj3DScanner::initialize()
{
  lineno = 1;
  pos = -1;
  advance();
}

void Obj3DScanner::advance()
{
  if (is_eof()) return;
  if (is_eol()) lineno++;
  pos++;
  skip_whitespace();
}

void Obj3DScanner::consume(char c)
{
  if (is_eof()) return;
  if (source[pos] != c) {
    std::cerr << "Obj3DScanner: Error in " << filename
              << ", unexpected character '" << source[pos]
              << "' at line " << lineno << std::endl;
  }
  advance();
}

void Obj3DScanner::skip_whitespace()
{
  while (is_whitespace()) {
    if (source[pos] == '#') {
      // Comment, scan until newline character is found
      while (!is_eof() && !is_eol()) { pos++; }
      advance(); // Consume the newline character as well
      continue;
    }
    pos++;
  }
}

std::string Obj3DScanner::get_keyword()
{
  std::string keyword = "";
  if (is_eol()) advance();
  while (is_alpha()) {
    keyword += source[pos++];
  }
  skip_whitespace();
  return keyword;
}

float Obj3DScanner::get_float()
{
  size_t length;
  float n = std::stof(source.substr(pos), &length);
  pos += length;
  skip_whitespace();
  return n;
}

int Obj3DScanner::get_integer()
{
  size_t length;
  //std::cout << "Obj3DScanner::get_integer() lineno=" << lineno << " str=" << source.substr(pos,5) << std::endl;
  int n = std::stoi(source.substr(pos), &length, 10);
  pos += length;
  skip_whitespace();
  return n;
}

bool Obj3DScanner::is_eof()
{
  return (pos == (int) source.size());
}

bool Obj3DScanner::is_eol()
{
  if (is_eof()) return false;
  return (source[pos] == '\n');
}

bool Obj3DScanner::is_alpha()
{
  if (is_eof()) return false;
  return (source[pos] >= 'a' && source[pos] <= 'z')
      || (source[pos] >= 'A' && source[pos] <= 'Z');
}

bool Obj3DScanner::is_numeric()
{
  if (is_eof()) return false;
  return (source[pos] >= '0' && source[pos] <= '9')
      || (source[pos] == '-');
}

bool Obj3DScanner::is_delimiter()
{
  if (is_eof()) return false;
  return (source[pos] == '/' || source[pos] == ',');
}

bool Obj3DScanner::is_whitespace()
{
  if (is_eof()) return false;
  return (source[pos] == '#' || source[pos] == ' ' || source[pos] == '\r');
}

