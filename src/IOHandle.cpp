#include "IOHandle.h"

#include <iostream>

IOHandle::IOHandle()
{
  //ctor
  std::cout << "IOHandle " << this << " created" << std::endl;
}

IOHandle::~IOHandle()
{
  //dtor
  std::cout << "IOHandle " << this << " destroyed" << std::endl;
}



