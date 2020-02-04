#ifndef IOHANDLE_H
#define IOHANDLE_H

#include <string>

class IOHandle
{
  public:
    IOHandle();
    virtual ~IOHandle();

    bool is_closed() { return closed; }
    int last_error() { return error; }
    virtual bool is_eof() { return false; }
    virtual std::string read(uint16_t bytes) { return ""; };
    virtual std::string readln() { return ""; };
    virtual std::string read() { return ""; };
    virtual void close() {};

  protected:
    bool closed = false;
    int error = 0;


  private:
};

#endif // IOHANDLE_H
