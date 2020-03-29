
#include "IO/IOFile.h"

#include <algorithm>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdio>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

//#define DEBUG_TRACE_IOFILE

IOFile::IOFile()
{
  //ctor
  filename = "";
  //handle = nullptr;
  bytes_read = -1;
#ifdef DEBUG_TRACE_IOFILE
  std::cout << "IOFile " << this << " created" << std::endl;
#endif
}

IOFile::~IOFile()
{
  //dtor
  if (handle.is_open()) handle.close();
  //std::cout << "IOFile " << this << " destroyed" << std::endl;
}


bool IOFile::is_eof()
{
  //std::cout << "IOFile::is_eof() bytes_read=" << bytes_read << " buffered=" << buffer.size() << std::endl;
  return (bytes_read == 0 && r_buffer.size() == 0);
}


bool IOFile::filemode_has(char c)
{
  return (this->filemode.find(c) != std::string::npos);
}


bool IOFile::is_writable()
{
  //std::cout << "IOFile::is_writable() openmode=" << openmode << std::endl;
  if (closed == true) return false;
  if (filemode_has('w')) return true;
  if (filemode_has('a')) return true;
  if (filemode_has('+')) return true;
  return false;
}


bool IOFile::is_readable()
{
  //std::cout << "IOFile::is_readable() openmode=" << openmode << std::endl;
  if (closed == true) return false;
  if (filemode_has('r')) return true;
  if (filemode_has('+')) return true;
  return false;
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


void IOFile::set_fileflags(std::string mode)
{
  this->filemode = mode;
  this->fileflags = std::ios::binary;
  if (filemode_has('r')) this->fileflags |= std::ios::in;
  if (filemode_has('w')) this->fileflags |= std::ios::out|std::ios::trunc;
  if (filemode_has('a')) this->fileflags |= std::ios::out|std::ios::app;
  if (filemode_has('+')) this->fileflags |= std::ios::out|std::ios::in;
}


// Static factory method
IOFile* IOFile::open(std::string filename, std::string mode) {
  //std::cout << "IOFile::open() filename='" << filename << "' mode='" << mode << "'" << std::endl;
  IOFile* file = new IOFile();
  bool res;

  file->filename = filename;
  res = file->r_buffer.empty();
  res = file->w_buffer.empty();
  file->set_fileflags(mode);

  file->handle.open(filename, file->fileflags);

  if (file->handle.fail()) {
    file->error = errno;
    file->closed = true;
    std::cerr << "IOFile::open() error "
              << errno << ":'" << strerror(errno)
              << "' opening '" << file->filename
              << "' in mode '" << file->filemode << "'"
              << std::endl;
  } else {
    file->error = 0;
    file->closed = false;
    //std::cout << "IOFile::open() filename='" << filename
    //          << "' mode='" << mode << "' OPENED OK as " << file << std::endl;
  }
  return file; // IOFile*
}

void IOFile::fill_buffer()
{
  //std::cout << "IOFile::fill_buffer()" << std::endl;
  if (!is_readable()) { this->bytes_read = 0; return; }

  std::size_t current_size = r_buffer.size();
  r_buffer.resize(current_size + this->bufsize);
  char* buffer_ptr = &r_buffer[current_size];
  this->handle.read(buffer_ptr, this->bufsize);

  if (this->handle.bad()) {
    this->error = errno;
    std::cerr << "IOFile error reading '" << filename << "':" << error << std::endl;
    this->closed = true;
    this->bytes_read = 0;
  } else {
    this->bytes_read = (size_t)this->handle.gcount();
  }
  if (bytes_read < this->bufsize) r_buffer.resize(current_size + bytes_read);

  //std::cout << "IOFile::fill_buffer() read " << bytes_read << " bytes from '" << this->filename << "'" << std::endl;
}

std::string IOFile::drain_buffer(const size_t bytes)
{
  //std::cout << "IOFile::drain_buffer() bytes=" << bytes << std::endl;
  size_t want_bytes = bytes;
  if (want_bytes > r_buffer.size()) want_bytes = r_buffer.size();

  std::string result(r_buffer.begin(), r_buffer.begin() + want_bytes);
  r_buffer.erase(r_buffer.begin(), r_buffer.begin() + want_bytes);
  return result;
}


size_t IOFile::flush_buffer(size_t bytes)
{
  //std::cout << "IOFile::flush_buffer() write " << bytes << " bytes" << std::endl;
  size_t bytes_to_write = bytes;
  if (bytes_to_write > this->w_buffer.size()) bytes_to_write = this->w_buffer.size();
  //std::cout << "IOFile::flush_buffer() can write " << bytes << " bytes" << std::endl;
  if (bytes_to_write == 0 || !is_writable()) return 0;

  //size_t bytes_written = fwrite(this->w_buffer.c_str(), sizeof(char), bytes_to_write, handle);
  char* buffer_ptr = &w_buffer[0];
  this->handle.write(buffer_ptr, bytes_to_write);
  size_t bytes_written = bytes_to_write;

  //std::cout << "IOFile::flush_buffer() wrote " << bytes_written << " bytes to '" << this->filename << "'" << std::endl;
  if (this->handle.bad()) {
    this->error = errno;
    std::cerr << "IOFile error writing '" << filename << "':" << error << std::endl;
    this->closed = true;
    this->w_buffer.clear();
    bytes_written = 0;
  }

  w_buffer.erase(w_buffer.begin(), w_buffer.begin() + bytes_written);
  return bytes_written;
}



std::string IOFile::getc()
{
  //std::cout << "IOFile::getc()" << std::endl;
  if (r_buffer.size() < this->bufsize) fill_buffer();
  return drain_buffer(1);
}

std::string IOFile::read(const int bytes)
{
  int read_bytes = bytes;
  //std::cout << "IOFile::read() bytes=" << bytes << std::endl;
  if (read_bytes <= 0) read_bytes = (int)this->bufsize;
  if (r_buffer.size() < this->bufsize) fill_buffer();
  return drain_buffer(read_bytes);
}

std::string IOFile::readln()
{
  std::string result;
  std::vector<char>::iterator pos;
  pos = std::find(r_buffer.begin(), r_buffer.end(), readln_newline);

  while (pos == r_buffer.end()) {
    //std::cout << "IOFile::readln() no newline in buffer, fill_buffer" << std::endl;
    fill_buffer();
    // Did we reach end-of-file?
    if (bytes_read == 0) {
      error = 0;
      result = drain_buffer(r_buffer.size()); // Flush buffer.
      //std::cout << "IOFile::readln() eof reached, flushed buffer: '" << result << "'" << std::endl;
      return result;
    }
    pos = std::find(r_buffer.begin(), r_buffer.end(), readln_newline);
  }
  //std::cout << "IOFile::readln() pos=" << pos << std::endl;
  error = 0;

  size_t bytes = std::distance(r_buffer.begin(), pos) + 1;
  result = drain_buffer(bytes); // Get line from buffer
  //std::cout << "IOFile::readln() drained buffer: '" << result << "'" << std::endl;
  return result;
}


int IOFile::write(const std::string data)
{
  //std::cout << "IOFile::write() bytes=" << data.length() << std::endl;
  if (!is_writable()) return 0;
  //this->w_buffer.append(data);
  std::copy(data.begin(), data.end(), std::back_inserter(w_buffer));
  // Combine small writes into 'bufsize' sized writes
  while ((int) this->w_buffer.size() >= this->bufsize) flush_buffer(this->bufsize);
  int bytes_written = (int)data.length();
  //std::cout << "IOFile::write() returning " << bytes_written << std::endl;
  return bytes_written;
}


int IOFile::writeln(const std::string data)
{
  //std::cout << "IOFile::writeln() bytes=" << data.length() << std::endl;
  if (!is_writable()) return 0;
  int bytes_written = write(data);
  bytes_written += write(newline);
  //std::cout << "IOFile::writeln() returning " << bytes_written << std::endl;
  return bytes_written;
}




void IOFile::close() {
  // In theory we should only ever have < 'bufsize' bytes left to write
  //std::cout << "IOFile::close() closing " << this << std::endl;
  //std::cout << "IOFile::close() " << w_buffer.size() << " bytes in write buffer" << std::endl;
  //std::cout << "IOFile::close() writable? " << (is_writable() ? "Yes" : "No") << std::endl;
  while (this->w_buffer.size() > 0 && is_writable()) flush_buffer(this->bufsize);
  //if (this->handle != nullptr) fclose(this->handle);
  this->handle.close();
  this->error = errno;
  this->r_buffer.clear();
  this->w_buffer.clear();
  this->filemode = "";
  this->closed = true;
  //this->handle = nullptr;
  //std::cout << "IOFile::close() " << this << " filename='" << filename << "' CLOSED OK" << std::endl;
}



// Convenience method for grabbing the contents of a small file
std::string IOFile::slurp(std::string filename)
{
#ifdef DEBUG_TRACE_IOFILE
  std::cout << "IOFile::slurp() filename=" << filename << std::endl;
#endif
  std::ifstream::pos_type fsize;
  std::ifstream fh (filename, std::ios::in|std::ios::binary|std::ios::ate);
  std::string buf = "";
  if (fh.is_open())
  {
    fsize = fh.tellg();
    //buf = new char [fsize+1];
    buf = std::string((size_t)fsize, '\0');
    fh.seekg (0, std::ios::beg);
    fh.read (&buf[0], fsize);
    fh.close();
    buf[(size_t)fsize] = 0;

#ifdef DEBUG_TRACE_IOFILE
    std::cout << "IOFile::slurp() read " << fsize << " bytes from " << filename << std::endl;
#endif
  } else {
    std::cerr << "IOFile::slurp() error reading " << filename << std::endl;
  }

  return buf;
}


std::string IOFile::get_path(const std::string filename)
{
  std::size_t slash = filename.find_last_of("/\\");
  if (slash == std::string::npos) return "";
  return filename.substr(0, slash+1);
}

