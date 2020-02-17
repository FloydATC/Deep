#ifndef IOHANDLE_H
#define IOHANDLE_H

#include <iostream>
#include <string>

#define ENOSTR 60 // Device not a stream

class IOHandle
{
  public:
    IOHandle();
    virtual ~IOHandle();

    bool is_closed() { return closed; }
    int last_error() { return error; }
    virtual bool is_eof() { return false; }
    virtual std::string read(const int bytes) { error=ENOSTR; return ""; }
    virtual std::string readln() { error=ENOSTR; return ""; }
    virtual int write(const std::string data) { error=ENOSTR; return -1; }
    virtual int writeln(const std::string data) { error=ENOSTR; return -1; }
    virtual std::string getc() { error=ENOSTR; return ""; }
    virtual std::string read() { error=ENOSTR; return ""; }
    virtual void close() {}

  protected:
    int bufsize = 4096;
    std::string buffer;
    std::string crlf = "\r\n";
    bool closed = false;
    int error = 0;

  private:
};

#endif // IOHANDLE_H
