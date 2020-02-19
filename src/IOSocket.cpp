#include "IOSocket.h"

#include <algorithm>

IOSocket::IOSocket()
{
  //ctor
  this->closed = true;
  this->protocol = "";
  this->sock_set = SDLNet_AllocSocketSet(1);
  this->error = 0;
}

IOSocket::~IOSocket()
{
  //dtor
  SDLNet_FreeSocketSet(this->sock_set);
}


bool IOSocket::is_eof()
{
  return (this->closed && r_buffer.size() == 0 && pr_buffer.size() == 0);
}


bool IOSocket::is_ready()
{
  return (SDLNet_CheckSockets(this->sock_set, 0) > 0);
}


// Static factory method
IOSocket* IOSocket::listen(uint16_t port, std::string protocol)
{
  IOSocket* socket = new IOSocket();

/*
  // create a listening TCP socket on port 9999 (server)
  IPaddress ip;
  TCPsocket tcpsock;

  if(SDLNet_ResolveHost(&ip,NULL,9999)==-1) {
      printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
      exit(1);
  }

  tcpsock=SDLNet_TCP_Open(&ip);
  if(!tcpsock) {
      printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
      exit(2);
  }
*/

  return socket;
}

// Static factory method
IOSocket* IOSocket::accept()
{
  IOSocket* socket = new IOSocket();

/*
  TCPsocket new_tcpsock;

  new_tcpsock=SDLNet_TCP_Accept(server_tcpsock);
  if(!new_tcpsock) {
      printf("SDLNet_TCP_Accept: %s\n", SDLNet_GetError());
  }
  else {
      // communicate over new_tcpsock
  }
*/

  return socket;
}

// Static factory method
IOSocket* IOSocket::connect(std::string host, uint16_t port, std::string protocol)
{
  IOSocket* socket = new IOSocket();
  socket->peer_host = host;
  socket->peer_port = port;
  socket->protocol = protocol;

  if (SDLNet_ResolveHost(&socket->peer_inetaddr, host.c_str(), port)==-1) {
    std::cerr << "SDLNet_ResolveHost: " << SDLNet_GetError() << std::endl;
    return socket;
  }

  if (protocol == "tcp") {
    socket->tcp_sock = SDLNet_TCP_Open(&socket->peer_inetaddr);
    if (!socket->tcp_sock) {
      std::cerr << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
      return socket;
    } else {
      std::cout << "Connected to " << host << ":" << protocol << "/" << port << std::endl;
      SDLNet_TCP_AddSocket(socket->sock_set, socket->tcp_sock);
      socket->protocol = protocol;
      socket->closed = false;
      return socket;
    }
  }

  if (protocol == "udp") {
    socket->udp_sock = SDLNet_UDP_Open(0);
    if(!socket->udp_sock) {
      std::cerr << "SDLNet_UDP_Open: " << SDLNet_GetError() << std::endl;
      return socket;
    } else {
      std::cout << "Connected to " << host << ":" << protocol << "/" << port << std::endl;
      SDLNet_UDP_AddSocket(socket->sock_set, socket->udp_sock);
      socket->protocol = protocol;
      socket->closed = false;
      return socket;
    }
  }

  std::cerr << "Unsupported protocol: '" << protocol << "'" << std::endl;
  return socket;
}


std::string IOSocket::read(const int bytes)
{
  if (this->protocol == "udp") return "";

  int read_bytes = bytes;
  //std::cout << "IOSocket::read() bytes=" << bytes << std::endl;
  if (read_bytes <= 0) read_bytes = this->bufsize;
  if ((int) r_buffer.size() < this->bufsize) fill_tcp_buffer();
  return drain_buffer(read_bytes);

}

std::string IOSocket::readln()
{
  if (this->protocol == "udp") return "";
  std::string result;
  std::vector<char>::iterator pos;
  pos = std::find(r_buffer.begin(), r_buffer.end(), readln_newline);

  if (pos == r_buffer.end() && is_ready()) {
    //std::cout << "IOSocket::readln() no newline in buffer, fill_buffer" << std::endl;
    fill_tcp_buffer();
    pos = std::find(r_buffer.begin(), r_buffer.end(), readln_newline);
  }
  //std::cout << "IOSocket::readln() pos=" << pos << std::endl;
  if (pos == r_buffer.end() && this->closed == false) {
    return ""; // No more data available yet
  }

  size_t bytes = std::distance(r_buffer.begin(), pos) + 1;
  result = drain_buffer(bytes); // Get line from buffer
  //std::cout << "IOSocket::readln() drained buffer: '" << result << "'" << std::endl;
  return result;
}



int IOSocket::write(const std::string data)
{
  if (this->protocol == "udp") return -1;

  //std::cout << "IOSocket::write() bytes=" << data.length() << std::endl;
  //this->w_buffer.append(data);
  std::copy(data.begin(), data.end(), std::back_inserter(this->w_buffer));
  // Try to flush immediately but back off if the connection is not ready
  //std::cout << "IOSocket::write() w_buffer size now " << this->w_buffer.size() << " bytes" << std::endl;
  while (this->w_buffer.size() > 0 && flush_tcp_buffer(this->bufsize) > 0) {
    //std::cout << "IOSocket::write() w_buffer size now " << this->w_buffer.size() << " bytes" << std::endl;
  };
  int bytes_written = data.length();
  //std::cout << "IOSocket::write() returning " << bytes_written << std::endl;
  return bytes_written;
}


int IOSocket::writeln(const std::string data)
{
  if (this->protocol == "udp") return -1;

  //std::cout << "IOSocket::writeln() bytes=" << data.length() << std::endl;
  int bytes_written = write(data);
  bytes_written += write(newline);
  //std::cout << "IOSocket::writeln() returning " << bytes_written << std::endl;
  return bytes_written;
}

std::string IOSocket::getc()
{
  //std::cout << "IOSocket::getc()" << std::endl;
  if ((int) r_buffer.size() < this->bufsize) fill_tcp_buffer();
  return drain_buffer(1);
}


int IOSocket::send(const std::string data)
{

  return 0;
}

std::string IOSocket::recv()
{


  return "";
}



void IOSocket::close()
{
  if (this->closed) return;
  if (this->protocol == "tcp") close_tcp();
  if (this->protocol == "udp") close_tcp();
  this->protocol = "";
  this->closed = true;
}


// Do not call directly, use close() instead
void IOSocket::close_tcp()
{
  SDLNet_TCP_DelSocket(this->sock_set, this->tcp_sock);
  SDLNet_TCP_Close(this->tcp_sock);
  this->r_buffer.clear();
  this->w_buffer.clear();
}

// Do not call directly, use close() instead
void IOSocket::close_udp()
{
  SDLNet_UDP_DelSocket(this->sock_set, this->udp_sock);
  SDLNet_UDP_Close(this->udp_sock);
  this->pr_buffer.clear();
  this->pw_buffer.clear();
}



void IOSocket::fill_udp_buffer()
{
  if (!is_ready()) return;

  std::size_t current_size = pr_buffer.size();
  pr_buffer.resize(current_size + 1);
  UDPpacket* buffer_ptr = &pr_buffer[current_size];

  int packets_read = SDLNet_UDP_Recv(this->udp_sock, buffer_ptr); // Non-blocking
  if (!packets_read) pr_buffer.resize(current_size);
}

void IOSocket::fill_tcp_buffer()
{
  if (!is_ready()) return;

  std::size_t current_size = r_buffer.size();
  r_buffer.resize(current_size + this->bufsize);
  char* buffer_ptr = &r_buffer[current_size];

  std::cout << "IOSocket::fill_tcp_buffer() calling SDLNet_TCP_Recv()..." << std::endl;
  int bytes_read = SDLNet_TCP_Recv(this->tcp_sock, buffer_ptr, this->bufsize);
  std::cout << "IOSocket::fill_tcp_buffer() read " << bytes_read << " bytes from '" << this->peer_host << "'" << std::endl;
  if (bytes_read < 0) {
    // An error may have occured, but sometimes you can just ignore it
    // It may be good to disconnect sock because it is likely invalid now.
    std::cerr << "IOSocket::fill_tcp_buffer() " << SDLNet_GetError() << std::endl;
    this->close();
    return;
  }

  if (bytes_read < this->bufsize) r_buffer.resize(current_size + bytes_read);
  //std::cout << "IOSocket::fill_buffer()  bytes_read=" << bytes_read << std::endl;
}

std::string IOSocket::drain_buffer(const size_t bytes)
{
  //std::cout << "IOSocket::drain_buffer() bytes=" << bytes << std::endl;
  size_t want_bytes = bytes;
  if (want_bytes > r_buffer.size()) want_bytes = r_buffer.size();

  std::string result(r_buffer.begin(), r_buffer.begin() + want_bytes);
  r_buffer.erase(r_buffer.begin(), r_buffer.begin() + want_bytes);
  return result;
}


size_t IOSocket::flush_tcp_buffer(size_t bytes)
{
  //std::cout << "IOSocket::flush_tcp_buffer() called" << std::endl;
  if (!is_ready()) return 0;

  //std::cout << "IOSocket::flush_tcp_buffer() write " << bytes << " bytes" << std::endl;
  size_t bytes_to_write = bytes;
  if (bytes_to_write > this->w_buffer.size()) bytes_to_write = this->w_buffer.size();
  //std::cout << "IOSocket::flush_tcp_buffer() can write " << bytes_to_write << " bytes" << std::endl;
  if (bytes_to_write == 0) return 0;

  //size_t bytes_written = fwrite(this->w_buffer.c_str(), sizeof(char), bytes_to_write, handle);
  char* buffer_ptr = &w_buffer[0];

  std::cout << "IOSocket::flush_tcp_buffer() calling SDLNet_TCP_Send()..." << std::endl;
  size_t bytes_written = SDLNet_TCP_Send(this->tcp_sock, buffer_ptr, bytes_to_write);
  //std::cout << "IOSocket::flush_buffer() wrote " << bytes_written << " bytes to '" << this->peer_host << "'" << std::endl;
  if (bytes_written < 0) {
    // An error may have occured, but sometimes you can just ignore it
    // It may be good to disconnect sock because it is likely invalid now.
    std::cerr << "IOSocket::flush_tcp_buffer() " << SDLNet_GetError() << std::endl;
    this->close();
    return bytes_written;
  }

  w_buffer.erase(w_buffer.begin(), w_buffer.begin() + bytes_written);
  return bytes_written;
}



