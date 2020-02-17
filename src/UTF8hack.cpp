#include <iostream>
#include <string.h>

#include "UTF8hack.h"


/*
UTF8hack::utf8()
{
  //ctor
}

UTF8hack::~utf8()
{
  //dtor
}
*/


char* UTF8hack::append(int cp, char* result, char* error)
{
  // TODO: Implement error checking
  //if (!UTF8hack::internal::is_code_point_valid(cp)) {
  //    error = UTF8hack::utf_error::INVALID_CODE_POINT;
  //    return result;
  //}
  //std::cout << "UTF8hack::append cp=" << cp << " result=" << &result << std::endl;
  //std::cout << "UTF8hack::append (char) of cp=" << (char) cp << std::endl;

  if (cp < 0x80)                        // one octet
    result[0] = static_cast<char>(cp);
  else if (cp < 0x800) {                // two octets
    result[0] = static_cast<char>((cp >> 6)            | 0xc0);
    result[1] = static_cast<char>((cp & 0x3f)          | 0x80);
  }
  else if (cp < 0x10000) {              // three octets
    result[0] = static_cast<char>((cp >> 12)           | 0xe0);
    result[1] = static_cast<char>(((cp >> 6) & 0x3f)   | 0x80);
    result[2] = static_cast<char>((cp & 0x3f)          | 0x80);
  }
  else {                                // four octets
    result[0] = static_cast<char>((cp >> 18)           | 0xf0);
    result[1] = static_cast<char>(((cp >> 12) & 0x3f)  | 0x80);
    result[2] = static_cast<char>(((cp >> 6) & 0x3f)   | 0x80);
    result[3] = static_cast<char>((cp & 0x3f)          | 0x80);
  }

  //std::cout << "UTF8hack::append returning " << &result << std::endl;
  return result;
}

int UTF8hack::get_codepoint(const char* str, int offset, int* bytes)
{
  int codepoint = 0xFFFD; // Invalid/unknown
  int len = strlen(str) - offset; // bytes
  if (len <= 0) { *bytes=0; return 0; }
  int i = offset;
  // Decode UTF8 into integer codepoint
  unsigned char u0 = str[i+0];
  if (u0>=0 && u0<=127) {
    *bytes=1; codepoint = u0; // Plain ASCII
  } else {
    if (len<2) { *bytes=1; return codepoint; } // Invalid/incomplete
    unsigned char u1 = str[i+1];
    if (u0>=192 && u0<=223) {
      *bytes=2; codepoint = (u0-192)*64 + (u1-128);
    } else {
      if (str[i+0]==0xed && (str[i+1] & 0xa0) == 0xa0) { *bytes=2; return codepoint; } //code points, 0xd800 to 0xdfff
      if (len<3) { *bytes=2; return codepoint; } // Invalid/incomplete
      unsigned char u2 = str[i+2];
      if (u0>=224 && u0<=239) {
        *bytes=3; codepoint = (u0-224)*4096 + (u1-128)*64 + (u2-128);
      } else {
        if (len<4) { *bytes=3; return codepoint; } // Invalid/incomplete
        unsigned char u3 = str[i+3];
        if (u0>=240 && u0<=247) {
          *bytes=4; codepoint = (u0-240)*262144 + (u1-128)*4096 + (u2-128)*64 + (u3-128);
        }
      }
    }
  }
  return codepoint;
}

std::vector<int> UTF8hack::codepoints(const char* str)
{
  std::vector<int> result;
  int len = strlen(str); // bytes
  int i = 0;
  int bytes = 0;
  while (i<len) {
    int codepoint = get_codepoint(str, i, &bytes);
    if (bytes == 0) break; // Unexpected error
    result.push_back(codepoint);
    i += bytes;
  }
  return result;
}

