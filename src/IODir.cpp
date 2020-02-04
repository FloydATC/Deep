#include "IODir.h"

#include <dirent.h>
#include <iostream>

IODir::IODir()
{
  //ctor
  handle = nullptr;
  //std::cout << "IODir() created" << std::endl;
}

IODir::~IODir()
{
  //dtor
  if (handle != nullptr) close();
  //std::cout << "IODir() destroyed" << std::endl;
}



// Static method
IODir* IODir::open(std::string dirname)
{
  IODir* dir = new IODir;
  dir->handle = ::opendir(dirname.c_str());
  dir->error = errno;
  if (dir->error) {
    dir->closed = true;
    std::cout << "IODir() error opening directory '" << dirname << "':" << dir->error << std::endl;
  } else {
    //std::cout << "IODir() opened" << std::endl;
    dir->closed = false;
  }
  return dir;
}

std::string IODir::read()
{
  if (handle == nullptr) return "";
  struct dirent* dp;
  if ((dp = ::readdir(handle)) != NULL) {
    return dp->d_name;
  } else {
    return "";
  }
}

void IODir::close()
{
  ::closedir(handle);
  handle = nullptr;
  closed = true;
  //std::cout << "IODir() closed" << std::endl;
}

