
#include "IOFile.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdio>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

IOFile::IOFile()
{
  //ctor
  filename = "";
  handle = nullptr;
  bytes_read = -1;
  //std::cout << "IOFile " << this << " created" << std::endl;
}

IOFile::~IOFile()
{
  //dtor
  if (handle != nullptr) close();
  //std::cout << "IOFile " << this << " destroyed" << std::endl;
}


bool IOFile::is_eof()
{
  //std::cout << "IOFile::is_eof() bytes_read=" << bytes_read << " buffered=" << buffer.size() << std::endl;
  return (bytes_read == 0 && buffer.size() == 0);
}


// Static method
IOFile* IOFile::stat(std::string filename) {
  IOFile* file = new IOFile();
  //std::cout << "IOFile::stat(" << filename << ")...";
  if (::stat(filename.c_str(), &file->statbuf) == -1) {
    //std::cout << "failed" << std::endl;
    file->error = errno;
    std::cerr << "IOFile error stating '" << filename << "':" << file->error << std::endl;
  } else {
    //std::cout << "ok" << std::endl;
    file->error = 0;
  }
  return file;
}


// Static method
IOFile* IOFile::open(std::string filename, std::string mode) {
  //std::cout << "IOFile::open() opening " << filename << std::endl;
  IOFile* file = new IOFile();

  file->filename = filename;
  file->buffer = "";
  file->openmode = mode;

  file->handle = fopen(filename.c_str(), mode.c_str());

  file->error = errno;
  if (file->error > 0) {
    std::cerr << "IOFile error opening '" << filename << "':" << file->error << std::endl;
    file->closed = true;
  } else {
    //std::cout << "IOFile::open() ok" << std::endl;
    file->closed = false;
  }
  return file;
}

void IOFile::fill_buffer()
{
  //std::cout << "IOFile::fill_buffer()" << std::endl;
  size_t buflen = this->bufsize;
  char buf[buflen];

  bytes_read = fread (buf, sizeof(char), buflen, handle);
  //std::cout << "IOFile::fill_buffer() bytes_read=" << bytes_read << std::endl;
  if (bytes_read == -1) {
    error = errno;
    std::cerr << "IOFile error reading '" << filename << "':" << error << std::endl;
    bytes_read = 0;
    closed = true;
  }
  if (bytes_read > 0) {
    //buffer += buf; // This would definitely break with 0 bytes
    buffer += std::string(buf, bytes_read); // Does this work?
  }
}

std::string IOFile::drain_buffer(int bytes)
{
  //std::cout << "IOFile::drain_buffer() bytes=" << bytes << std::endl;
  if (bytes > (int) buffer.size()) bytes = buffer.size();
  std::string result = buffer.substr(0, bytes);
  buffer = buffer.substr(bytes);
  return result;
}

std::string IOFile::getc()
{
  //std::cout << "IOFile::getc()" << std::endl;
  if ((int) buffer.size() < this->bufsize) fill_buffer();
  return drain_buffer(1);
}

std::string IOFile::read(const int bytes)
{
  int read_bytes = bytes;
  //std::cout << "IOFile::read() bytes=" << bytes << std::endl;
  if (read_bytes <= 0) read_bytes = this->bufsize;
  if ((int) buffer.size() < this->bufsize) fill_buffer();
  return drain_buffer(read_bytes);
}

std::string IOFile::readln() {
  std::string result;
  std::string::size_type pos;
  pos = buffer.find("\n");
  if (pos == std::string::npos) {
    fill_buffer();
    // Did we reach end-of-file?
    if (bytes_read == 0) {
      error = 0;
      return drain_buffer(buffer.size()); // Flush buffer.
    }
  }

  pos = buffer.find("\n");
  if (pos == std::string::npos) {
    error = EAGAIN;
    return ""; // No newline found, end-of-file not yet detected
  } else {
    error = 0;
    return drain_buffer(pos + 1); // Return line from buffer.
  }
}


int IOFile::write(const std::string data) {
  this->buffer.append(data);
  return data.length();
}


int IOFile::writeln(const std::string data) {
  this->buffer.append(data);
  this->buffer.append(this->crlf);
  return data.length() + this->crlf.length();
}




void IOFile::close() {
  fclose(handle);
  handle = nullptr;
  buffer = "";
  closed = true;
  //std::cout << "IOFile::close() " << this << " closed " << filename << std::endl;
}



// Convenience method for grabbing the contents of a small file
std::string IOFile::slurp(std::string filename)
{
  std::ifstream::pos_type fsize;
  std::string buf = "";
  std::ifstream fh (filename, std::ios::in|std::ios::binary|std::ios::ate);
  if (fh.is_open())
  {
    fsize = fh.tellg();
    //buf = new char [fsize+1];
    buf = std::string(fsize, '\0');
    fh.seekg (0, std::ios::beg);
    fh.read (&buf[0], fsize);
    fh.close();
    buf[fsize] = 0;

    //std::cout << "File::load_file() read " << fsize << " bytes from " << filename << std::endl;
  } else {
    std::cerr << "File::load_file() error reading " << filename << std::endl;
  }

  return buf;
}



