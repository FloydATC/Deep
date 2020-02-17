#include "IOStream.h"

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

std::string IOStream::drain_buffer(int bytes)
{
  //std::cout << "IOStream::drain_buffer() bytes=" << bytes << std::endl;
  if (bytes > (int) buffer.size()) bytes = buffer.size();
  std::string result = buffer.substr(0, bytes);
  buffer = buffer.substr(bytes);
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
  if (read_bytes <= 0) read_bytes = this->bufsize;
  return drain_buffer(read_bytes);
}

std::string IOStream::readln() {
  std::string::size_type pos;
  pos = buffer.find("\n");
  if (pos == std::string::npos) {
    error = EAGAIN;
    return ""; // No newline found, end-of-file not yet detected
  } else {
    error = 0;
    return drain_buffer(pos + 1); // Return line from buffer.
  }
}


int IOStream::write(const std::string data) {
  this->buffer.append(data);
  return data.length();
}


int IOStream::writeln(const std::string data) {
  this->buffer.append(data);
  this->buffer.append(this->crlf);
  return data.length() + this->crlf.length();
}


void IOStream::close() {
  this->closed = true;
}

