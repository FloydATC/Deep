#include "IODir.h"

#include <dirent.h>
#include <iostream>

IODir::IODir()
{
  //ctor
  handle = nullptr;
  dirname = "";
  //std::cout << "IODir() " << this << " created" << std::endl;
}

IODir::~IODir()
{
  //dtor
  if (handle != nullptr) close();
  //std::cout << "IODir() " << this << " destroyed" << std::endl;
}



// Static method
IODir* IODir::open(std::string dirname)
{
  //std::cout << "IODir::open() opening " << dirname << std::endl;
  IODir* dir = new IODir;
  dir->dirname = dirname;
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
  //std::cout << "IODir::read(" << handle <<  ")...";
  if ((dp = ::readdir(handle)) != NULL) {
    //std::cout << "ok" << std::endl;
    return dp->d_name;
  } else {
    //std::cout << "returned null" << std::endl;
    return "";
  }
}

void IODir::close()
{
  ::closedir(handle);
  handle = nullptr;
  closed = true;
  //std::cout << "IODir::close() closed " << dirname << std::endl;
}

