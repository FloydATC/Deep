#ifndef IODIR_H
#define IODIR_H

#include <dirent.h>
#include <string>

#include "IOHandle.h"

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
    DIR* handle;

};

#endif // IODIR_H
