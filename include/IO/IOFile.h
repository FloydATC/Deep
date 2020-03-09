#ifndef IOFILE_H
#define IOFILE_H

#include <fstream>
#include <vector>
#include <string>

#include "IO/IOHandle.h"
#include "sys/stat.h"


class IOFile : public IOHandle
{
  public:
    IOFile();
    ~IOFile();

    static IOFile* open(std::string filename, std::string mode);
    static IOFile* stat(std::string filename);
    std::string read(const int bytes);
    std::string readln();
    int write(const std::string data);
    int writeln(const std::string data);
    std::string getc();
    void close();
    bool is_eof();
    bool is_readable();
    bool is_writable();
    int uid() { return statbuf.st_uid; };
    int gid() { return statbuf.st_gid; };
    int mode() { return statbuf.st_mode; };
    int size() { return statbuf.st_size; };
    int atime() { return statbuf.st_atime; };
    int mtime() { return statbuf.st_mtime; };
    int ctime() { return statbuf.st_ctime; };

    std::string filename;

    static std::string slurp(std::string filename);
    static std::string get_path(const std::string filename);

  protected:

  private:
    struct stat statbuf;
    //FILE* handle;
    std::fstream handle;
    std::string filemode; // fopen style "rwa+" visible to user
    std::ios_base::openmode fileflags; // internal std::fstream madness
    ssize_t bytes_read;

    void fill_buffer();
    std::string drain_buffer(const size_t bytes);
    size_t flush_buffer(const size_t bytes);
    bool filemode_has(char c);
    void set_fileflags(std::string mode);

};

#endif // IOFILE_H
