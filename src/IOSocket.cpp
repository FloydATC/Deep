#include "IOSocket.h"

#include <algorithm>



IOSocket::IOSocket()
{
  //ctor
  this->closed = true;
  this->protocol = "";
  this->error = 0;
  ws_initialize();
}

IOSocket::~IOSocket()
{
  //dtor

}


bool IOSocket::is_eof()
{
  return (this->closed && r_buffer.size() == 0 && pr_buffer.size() == 0);
}

bool IOSocket::is_closed()
{
  return (this->closed && r_buffer.size() == 0 && pr_buffer.size() == 0);
}

bool IOSocket::is_ready()
{
  //return (SDLNet_CheckSockets(this->sock_set, 0) > 0);
  return true;
}



// Static factory method
IOSocket* IOSocket::listen(std::string port, std::string protocol)
{
  IOSocket* socket = new IOSocket();


  return socket;
}

// Static factory method
IOSocket* IOSocket::accept()
{
  IOSocket* socket = new IOSocket();


  return socket;
}

// Static factory method
IOSocket* IOSocket::connect(std::string host, std::string port, std::string protocol)
{
  IOSocket* socket = new IOSocket();
  socket->peer_host = host;
  socket->peer_port = port;
  socket->protocol = protocol;

  if (socket->ws_resolve_address(host, port) == false) {
      socket->error = -1; // Address lookup failure
      return socket;
  }

  if (protocol == "tcp") {
    if (socket->ws_create_tcp_socket() == false) {
      socket->error = -2; // Error creating TCP socket
      return socket;
    }
  } else if (protocol == "udp") {
    if (socket->ws_create_udp_socket() == false) {
      socket->error = -3; // Error creating UDP socket
      return socket;
    }
  } else {
    socket->error = -4; // Bad protocol
    socket->protocol = "";
    return socket;
  }

  if (socket->ws_set_nonblocking() == false) {
    socket->error = -5; // Could not set non-blocking mode
    return socket;
  }

  if (socket->ws_connect() == false) {
    socket->error = -6; // Connect failed
    return socket;
  }

  // Note: The socket may not actually be connected yet
  // because we set it to non-blocking mode, but atleast
  // we know it was not immediately declined.
  // Subsequent send() and recv() operations may fail with
  // certain error codes that mean the connection is
  // not yet ready. Use ws_is_retry() to test for these.

  socket->closed = false;
  //std::cout << "IOSocket::connect() ok" << std::endl;
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

  if (pos == r_buffer.end() && this->closed == false) {
    //std::cout << "IOSocket::readln() no newline in buffer, fill_buffer" << std::endl;
    fill_tcp_buffer();
    pos = std::find(r_buffer.begin(), r_buffer.end(), readln_newline);
  }
  //std::cout << "IOSocket::readln() pos=" << pos << std::endl;
  if (pos == r_buffer.end() && this->closed == false) {
    //std::cout << "IOSocket::readln() no data available yet" << std::endl;
    return ""; // No more data available yet
  }

  // If we get this far it means EITHER we would a newline at 'pos',
  // OR the connection closed and 'pos' points to the end of the buffer.
  // Add 1 because distance() is an offset and we want the size.
  size_t bytes = std::distance(r_buffer.begin(), pos) + 1;
  result = drain_buffer(bytes); // Get line from buffer
  //std::cout << "IOSocket::readln() drained buffer: '" << result << "'" << std::endl;
  return result;
}



int IOSocket::write(const std::string data)
{
  if (this->protocol == "udp") return -1;
  if (this->closed) return -1;

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
  std::string line = data;
  int bytes_written = write(line.append(newline));
  //bytes_written += write(newline);
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

  // Send a packet from pw_buffer

  return 0;
}

std::string IOSocket::recv()
{

  // Receive a packet into pr_buffer

  return "";
}



void IOSocket::close()
{
  if (this->closed) return;
  if (this->protocol == "tcp") close_tcp();
  if (this->protocol == "udp") close_tcp();
  ws_close();
  this->closed = true;
}


// Do not call directly, use close() instead
void IOSocket::close_tcp()
{
  // Try reading to see if there is data waiting

  this->w_buffer.clear();
}

// Do not call directly, use close() instead
void IOSocket::close_udp()
{
  /*
  this->pw_buffer.clear();
  */
}



void IOSocket::fill_udp_buffer()
{
  if (this->closed) return;

  //std::size_t current_size = pr_buffer.size();
  //pr_buffer.resize(current_size + 1);
  //UDPpacket* buffer_ptr = &pr_buffer[current_size];

  //int packets_read = SDLNet_UDP_Recv(this->udp_sock, buffer_ptr); // Non-blocking
  //if (!packets_read) pr_buffer.resize(current_size);
}

void IOSocket::fill_tcp_buffer()
{
  if (this->closed) return;

  //std::cout << "IOSocket::fill_tcp_buffer() calling ws_recv" << std::endl;
  std::string bytes = ws_recv();
  //std::cout << "IOSocket::fill_tcp_buffer() read " << bytes.length() << " bytes" << std::endl;

  if (bytes.length() > 0) {
    std::copy(bytes.begin(), bytes.end(), std::back_inserter(r_buffer));
  }
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
  if (this->closed) return 0;

  //std::cout << "IOSocket::flush_tcp_buffer() write " << bytes << " bytes" << std::endl;
  size_t bytes_to_write = bytes;
  if (bytes_to_write > this->w_buffer.size()) bytes_to_write = this->w_buffer.size();
  //std::cout << "IOSocket::flush_tcp_buffer() can write " << bytes_to_write << " bytes" << std::endl;
  if (bytes_to_write == 0) return 0;

  //std::cout << "IOSocket::flush_tcp_buffer() calling ws_send to write " << bytes_to_write << " bytes" << std::endl;
  size_t bytes_written = ws_send(std::string(w_buffer.begin(), w_buffer.begin() + bytes_to_write));
  //std::cout << "IOSocket::flush_tcp_buffer() wrote " << bytes_written << " bytes" << std::endl;
  if (bytes_written == 0) return 0;

  w_buffer.erase(w_buffer.begin(), w_buffer.begin() + bytes_written);
  return bytes_written;
}


// ==== winsock specific methods ====
// https://docs.microsoft.com/nb-no/windows/win32/api/winsock


bool IOSocket::ws_initialize()
{
  int res = WSAStartup(MAKEWORD(2,2), &this->wsaData);
  if (res != 0) {
    std::cerr << "IOSocket::init_winsock() WSAStartup failed: " << res << ", " << WSAGetLastError() << std::endl;
    return false;
  }
  return true;
}


bool IOSocket::ws_resolve_address(std::string hostname, std::string port)
{
  bool result = false;

  if (this->peer_addr != nullptr) {
    freeaddrinfo(this->peer_addr);
    this->peer_addr = nullptr;
  }
  struct addrinfo hints = {0};
  hints.ai_family = AF_INET;

  int code = getaddrinfo(hostname.c_str(), port.c_str(), &hints, &this->peer_addr);

  if (code == 0 && this->peer_addr != nullptr) {
    result = true;
    //std::cout << "IOSocket::ws_resolve_address() getaddrinfo ok (code=" << code << ")" << std::endl;
  } else {
    result = false;
    std::cerr << "IOSocket::resolve_address() getaddrinfo failed: " << ws_errormessage(code) << std::endl;
    WSACleanup();
  }

  return result;
}


bool IOSocket::ws_create_tcp_socket()
{
  this->host_addr.ai_addr     = INADDR_ANY;
  this->host_addr.ai_family   = AF_INET;
  this->host_addr.ai_socktype = SOCK_STREAM;
  this->host_addr.ai_protocol = IPPROTO_TCP;

  this->handle = ::socket(this->host_addr.ai_family,
                          this->host_addr.ai_socktype,
                          this->host_addr.ai_protocol);

  if (this->handle == INVALID_SOCKET) {
    std::cerr << "IOSocket::create_tcp_socket() socket failed: " << WSAGetLastError() << std::endl;
    WSACleanup();
    return false;
  }

  //std::cout << "IOSocket::create_tcp_socket() ok" << std::endl;
  return true;
}


bool IOSocket::ws_create_udp_socket()
{
  this->host_addr.ai_family   = AF_INET;
  this->host_addr.ai_socktype = SOCK_DGRAM;
  this->host_addr.ai_protocol = IPPROTO_UDP;

  this->handle = ::socket(this->host_addr.ai_family,
                          this->host_addr.ai_socktype,
                          this->host_addr.ai_protocol);

  if (this->handle == INVALID_SOCKET) {
    std::cerr << "IOSocket::create_tcp_socket() socket failed: " << WSAGetLastError() << std::endl;
    WSACleanup();
    return false;
  }

  //std::cout << "IOSocket::create_udp_socket() ok" << std::endl;
  return true;
}


bool IOSocket::ws_set_nonblocking()
{
  //std::cout << "IOSocket::ws_set_nonblocking()" << std::endl;
  u_long mode = 1;  // 1 to enable non-blocking socket
  ioctlsocket(this->handle, FIONBIO, &mode);
  return true;
}


bool IOSocket::ws_connect()
{
  bool result = false;
  int status = ::connect(this->handle,
                      this->peer_addr->ai_addr,
                (int) this->peer_addr->ai_addrlen);

  if (status != SOCKET_ERROR) {
    //std::cout << "IOSocket::ws_connect() connected ok" << std::endl;
    result = true;
  } else {
    int code = WSAGetLastError();
    if (ws_is_retry(code)) {
      //std::cout << "IOSocket::ws_connect() connect in progress" << std::endl;
      result = true;
    } else if (ws_is_error(code)) {
      std::cerr << "IOSocket::ws_connect() connect failed: " << ws_errormessage(code) << std::endl;
      this->close();
      result = false;
    } else {
      std::cerr << "IOSocket::ws_connect() connect returned unknown code: " << code << std::endl;
      this->close();
      result = false;
    }
  }

  return result;
}

size_t IOSocket::ws_send(std::string data)
{
  size_t bytes_sent = 0;
  int status = ::send(this->handle, data.c_str(), data.length(), 0);

  if (status != SOCKET_ERROR) {
    bytes_sent = status;
    //std::cout << "IOSocket::ws_send() sent " << bytes_sent << " bytes ok" << std::endl;
  } else {
    int code = WSAGetLastError();
    if (ws_is_retry(code)) {
      bytes_sent = 0;
      //std::cout << "IOSocket::ws_send() send would block" << std::endl;
    } else if (ws_is_error(code)) {
      bytes_sent = 0;
      std::cerr << "IOSocket::ws_send() send failed: " << ws_errormessage(code) << std::endl;
      this->close();
    } else {
      bytes_sent = 0;
      std::cerr << "IOSocket::ws_send() send returned unknown code: " << code << std::endl;
      this->close();
    }
  }
  // IOSocket::ws_send() send returned unknown code: 10053

  return bytes_sent;
}

std::string IOSocket::ws_recv()
{
  std::string result;
  char* buf{ new char[this->bufsize]{} };
  int status = ::recv(this->handle, buf, this->bufsize, 0);

  if (status != SOCKET_ERROR) {
    size_t bytes_received = status;
    result = std::string(buf, bytes_received);
    //std::cout << "IOSocket::ws_recv() received " << bytes_received << " bytes ok" << std::endl;
  } else {
    int code = WSAGetLastError();
    if (ws_is_retry(code)) {
      result = "";
      //std::cout << "IOSocket::ws_send() recv would block" << std::endl;
    } else if (ws_is_error(code)) {
      result = "";
      std::cerr << "IOSocket::ws_recv() recv failed: " << ws_errormessage(code) << std::endl;
      this->close();
    } else {
      result = "";
      std::cerr << "IOSocket::ws_recv() recv returned unknown code: " << code << std::endl;
      this->close();
    }
  }

  delete[] buf;
  return result;
}



// Do not call directly, use close() instead
bool IOSocket::ws_close()
{
  //std::cout << "IOSocket::ws_close()" << std::endl;
  if (this->handle == INVALID_SOCKET) return false;
  ::closesocket(this->handle);
  WSACleanup();
  this->handle = INVALID_SOCKET;
  return true;
}

bool IOSocket::ws_is_retry(int result)
{
  if (result == WSAEINPROGRESS)        return true; // Call is in progress
  if (result == WSAEALREADY)           return true; // A non-blocking connect has not completed yet
  if (result == WSAEWOULDBLOCK)        return true; // Not ready, try again
  if (result == WSATRY_AGAIN)          return true; // Try again
  if (result == WSAENOTCONN)           return true; // Not connected
  return false;
}

bool IOSocket::ws_is_error(int result)
{
  if (result == WSANOTINITIALISED)     return true; // WSAStartup failed or not called
  if (result == WSAENETDOWN)           return true; // The network subsystem has failed
  if (result == WSAENETRESET)          return true; // Connection reset
  if (result == WSAEADDRINUSE)         return true; // Address already in use
  if (result == WSAEINTR)              return true; // Cancelled with WSACancelBlockingCall
  if (result == WSAEADDRNOTAVAIL)      return true; // Remote address is invalid
  if (result == WSAEAFNOSUPPORT)       return true; // Invalid address for this address family
  if (result == WSAECONNREFUSED)       return true; // Connection refused
  if (result == WSAEFAULT)             return true; // Invalid socket address
  if (result == WSAEINVAL)             return true; // Socket is in listen mode
  if (result == WSAEISCONN)            return true; // Socket already connected
  if (result == WSAENETUNREACH)        return true; // Network unreachable
  if (result == WSAEHOSTUNREACH)       return true; // Host unreachable
  if (result == WSAENOBUFS)            return true; // No buffer space
  if (result == WSAENOTSOCK)           return true; // Not a socket
  if (result == WSAETIMEDOUT)          return true; // Connection timeout
  if (result == WSAEACCES)             return true; // Broadcast not enabled
  if (result == WSA_NOT_ENOUGH_MEMORY) return true; //
  if (result == WSAESOCKTNOSUPPORT)    return true; //
  if (result == WSAHOST_NOT_FOUND)     return true; //
  if (result == WSANO_DATA)            return true; //
  if (result == WSANO_RECOVERY)        return true; // A nonrecoverable error occurred (thank you, Windows)
  if (result == WSATYPE_NOT_FOUND)     return true; //
  if (result == WSAESHUTDOWN)          return true; // Connection shut down
  if (result == WSAECONNABORTED)       return true; // Connection aborted
  if (result == WSAECONNRESET)         return true; // Connection reset
  if (result == WSAETIMEDOUT)          return true; // Connection timeout
  return false;
}

std::string IOSocket::ws_errormessage(int result)
{
  // Transient errors
  if (result == WSAEINPROGRESS)        return "WSAEINPROGRESS";
  if (result == WSAEALREADY)           return "WSAEALREADY";
  if (result == WSAEWOULDBLOCK)        return "WSAEWOULDBLOCK";
  if (result == WSATRY_AGAIN)          return "WSATRY_AGAIN";
  if (result == WSAENOTCONN)           return "WSAENOTCONN";

  // Permanent errors
  if (result == WSAEACCES)             return "WSAEACCES";
  if (result == WSAENETDOWN)           return "WSAENETDOWN";
  if (result == WSAEADDRINUSE)         return "WSAEADDRINUSE";
  if (result == WSAEADDRNOTAVAIL)      return "WSAEADDRNOTAVAIL";
  if (result == WSAEAFNOSUPPORT)       return "WSAEAFNOSUPPORT";
  if (result == WSAECONNREFUSED)       return "WSAECONNREFUSED";
  if (result == WSAECONNABORTED)       return "WSAECONNABORTED";
  if (result == WSAECONNRESET)         return "WSAECONNRESET";
  if (result == WSAEFAULT)             return "WSAEFAULT";
  if (result == WSAEINTR)              return "WSAEINTR";
  if (result == WSAEISCONN)            return "WSAEISCONN";
  if (result == WSAEINVAL)             return "WSAEINVAL";
  if (result == WSAENETUNREACH)        return "WSAENETUNREACH";
  if (result == WSAENETRESET)          return "WSAENETRESET";
  if (result == WSAEHOSTUNREACH)       return "WSAEHOSTUNREACH";
  if (result == WSAENOBUFS)            return "WSAENOBUFS";
  if (result == WSAENOTSOCK)           return "WSAENOTSOCK";
  if (result == WSAETIMEDOUT)          return "WSAETIMEDOUT";
  if (result == WSANOTINITIALISED)     return "WSANOTINITIALISED";
  if (result == WSA_NOT_ENOUGH_MEMORY) return "WSA_NOT_ENOUGH_MEMORY";
  if (result == WSANO_DATA)            return "WSANO_DATA";
  if (result == WSANO_RECOVERY)        return "WSANO_RECOVERY";
  if (result == WSAESOCKTNOSUPPORT)    return "WSAESOCKTNOSUPPORT";
  if (result == WSAHOST_NOT_FOUND)     return "WSAHOST_NOT_FOUND";
  if (result == WSATYPE_NOT_FOUND)     return "WSATYPE_NOT_FOUND";
  if (result == WSAESHUTDOWN)          return "WSAESHUTDOWN";
  if (result == WSAETIMEDOUT)          return "WSAETIMEDOUT";

  return std::string("UNKNOWN=").append(std::to_string(result));
}
