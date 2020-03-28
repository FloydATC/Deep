#ifndef IOHANDLE_H
#define IOHANDLE_H

#include <iostream>
#include <string>
#include <vector>

#ifndef ENOSTR
#define ENOSTR 60 // Device not a stream
#endif

class IOHandle
{
  public:
    IOHandle();
    virtual ~IOHandle();

    std::string strip_cr_lf(std::string str);

    virtual bool is_closed() { return closed; }
    virtual int last_error() { return error; }
    virtual bool is_eof() { return false; }
    virtual bool is_readable() { return false; }
    virtual bool is_writable() { return false; }
    virtual std::string read(const int bytes) { error=ENOSTR; return ""; }
    virtual std::string readln() { error=ENOSTR; return ""; }
    virtual int write(const std::string data) { error=ENOSTR; return -1; }
    virtual int writeln(const std::string data) { error=ENOSTR; return -1; }
    virtual std::string getc() { error=ENOSTR; return ""; }
    virtual std::string read() { error=ENOSTR; return ""; }
    virtual IOHandle* accept() { error=ENOSTR; return nullptr; }
    virtual int send(std::string data) { error=ENOSTR; return -1; }
    virtual std::string recv() { error=ENOSTR; return ""; }

    virtual void close() {}

  protected:
    size_t bufsize = 4096;
    std::vector<char> r_buffer;
    std::vector<char> w_buffer;
    char readln_newline = '\n';
#if defined _WIN32 || defined _WIN32_WINNT || defined _WIN64 || defined WIN32
#define IS_WINDOWS
    std::string newline = "\r\n";
#endif
#ifndef IS_WINDOWS
    std::string newline = "\n";
#endif
    bool closed = false;
    int error = 0;

  private:
};

#endif // IOHANDLE_H
