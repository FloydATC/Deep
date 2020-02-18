#ifndef IOSTREAM_H
#define IOSTREAM_H

#include "IOHandle.h"

class IOStream : public IOHandle
{
  public:
    IOStream();
    ~IOStream();

    void close();
    std::string read(const int bytes);
    std::string readln();
    int write(const std::string data);
    int writeln(const std::string data);
    std::string getc();

  protected:

  private:
    std::string drain_buffer(const size_t bytes);
};

#endif // IOSTREAM_H
