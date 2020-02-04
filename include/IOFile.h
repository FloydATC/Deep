#ifndef IOFILE_H
#define IOFILE_H

#include <vector>
#include <string>

#include "IOHandle.h"



class IOFile : public IOHandle
{
  public:
    IOFile();
    ~IOFile();

    static IOFile* open(std::string filename, std::string mode);
    static IOFile* stat(std::string filename);
    std::string read(uint16_t bytes);
    std::string readln();
    void close();
    bool is_eof();
    int uid() { return statbuf.st_uid; };
    int gid() { return statbuf.st_gid; };
    int mode() { return statbuf.st_mode; };
    int size() { return statbuf.st_size; };
    int atime() { return statbuf.st_atime; };
    int mtime() { return statbuf.st_mtime; };
    int ctime() { return statbuf.st_ctime; };

    std::string filename;

    static std::string slurp(std::string filename);

  protected:

  private:
    struct stat statbuf;
    FILE* handle;
    std::string openmode;
    std::string buffer;
    ssize_t bytes_read;

    void fill_buffer();
    std::string drain_buffer(uint16_t bytes);

};

#endif // IOFILE_H
