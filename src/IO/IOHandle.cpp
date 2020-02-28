#include "IO/IOHandle.h"

#include <iostream>
#include <string>

IOHandle::IOHandle()
{
  //ctor
  //std::cout << "IOHandle " << this << " created" << std::endl;
}

IOHandle::~IOHandle()
{
  //dtor
  //std::cout << "IOHandle " << this << " destroyed" << std::endl;
}

std::string IOHandle::strip_cr_lf(std::string str)
{
  size_t cr = str.find_first_of("\r");
  if (cr != std::string::npos) str.erase(cr, 1);
  size_t lf = str.find_first_of("\n");
  if (lf != std::string::npos) str.erase(lf, 1);
  return str;
}

