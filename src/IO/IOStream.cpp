#include "IO/IOStream.h"

#include <algorithm>
#include <iostream>
#include <string>

IOStream::IOStream()
{
  //ctor
  this->closed = false; // A stream is open until discarded
}

IOStream::~IOStream()
{
  //dtor
}

std::string IOStream::drain_buffer(const size_t bytes)
{
  //std::cout << "IOStream::drain_buffer() bytes=" << bytes << std::endl;
  size_t want_bytes = bytes;
  if (want_bytes > r_buffer.size()) want_bytes = r_buffer.size();
  std::string result(r_buffer.begin(), r_buffer.begin() + want_bytes);
  r_buffer.erase(r_buffer.begin(), r_buffer.begin() + want_bytes);
  //std::cout << "IOStream::drain_buffer() returning '" << result << "'" << std::endl;
  return result;
}

std::string IOStream::getc()
{
  //std::cout << "IOStream::getc()" << std::endl;
  return drain_buffer(1);
}

std::string IOStream::read(int bytes)
{
  int read_bytes = bytes;
  //std::cout << "IOStream::read() bytes=" << bytes << std::endl;
  if (read_bytes <= 0) read_bytes = (int)this->bufsize;
  return drain_buffer(read_bytes);
}

std::string IOStream::readln() {
  std::vector<char>::iterator pos;
  pos = std::find(r_buffer.begin(), r_buffer.end(), readln_newline);
  if (pos == r_buffer.end()) {
    error = EAGAIN;
    return ""; // No newline found, end-of-file not yet detected
  } else {
    error = 0;
    return drain_buffer(std::distance(r_buffer.begin(), pos) + 1); // Return line from buffer.
  }
}


int IOStream::write(const std::string data) {
  std::copy(data.begin(), data.end(), std::back_inserter(w_buffer));
  return (int)data.length();
}


int IOStream::writeln(const std::string data) {
  int bytes_written = write(data);
  bytes_written += write(newline);
  return bytes_written;
}


void IOStream::close() {
  this->closed = true;
}

