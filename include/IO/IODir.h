#ifndef IODIR_H
#define IODIR_H

#include <dirent.h>
#include <string>

#include "IO/IOHandle.h"

class IODir : public IOHandle
{
  public:
    IODir();
    ~IODir();

    static IODir* open(std::string dirname);
    std::string read();
    void close();

  protected:

  private:
    std::string dirname;
    DIR* handle;

};

#endif // IODIR_H
