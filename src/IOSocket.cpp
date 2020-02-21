#include "IOSocket.h"

#include <algorithm>



IOSocket::IOSocket()
{
  //ctor
  this->closed = true;
  this->protocol = "";
  this->error = 0;
}

IOSocket::~IOSocket()
{
  //dtor
}


bool IOSocket::is_eof()
{
  // Even if a socket has been closed, there may be data left to read
  return (this->closed && r_buffer.size() == 0 && pr_buffer.size() == 0);
}

bool IOSocket::is_closed()
{
  // Even if a socket has been closed, there may be data left to read
  return (this->closed && r_buffer.size() == 0 && pr_buffer.size() == 0);
}


// Public static factory method
IOSocket* IOSocket::listen(std::string port, std::string protocol)
{
  IOSocket* socket = new IOSocket();

  // Numerous examples online, including MSFT ones, show casting
  // to sockaddr_in and direct manipulation like
  //   .sin_addr.s_addr=INADDR_ANY;
  //   .sin_port = htons(port);
  // Winsock would consistently ignore the port number and instead
  // bind a random port, which is pretty useless for a server.
  // This is why I had to go with the following approach:
  if (socket->ws_resolve_address(std::string("0.0.0.0"), port) == false) return socket;
  socket->host_addr = socket->resolved[0];

  if (protocol == "tcp" && socket->ws_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) == false) return socket;
  if (protocol == "udp" && socket->ws_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) == false) return socket;
  if (protocol != "tcp" && protocol != "udp") { socket->error = -1; return socket; }

  if (socket->ws_set_reuseaddr() == false) return socket;
  if (socket->ws_set_nonblocking() == false) return socket;
  if (socket->ws_bind() == false) return socket;
  socket->listen_pending = true;

  // Because the socket is non-blocking, listen() probably would not be able to complete just yet.
  // We will therefore try calling it when the user calls accept() to check for connections.

  return socket;
}

// Public static factory method
IOSocket* IOSocket::accept()
{
  if (this->listen_pending) {
    //std::cout << "IOSocket" << this << "::accept() listen is pending" << std::endl;
    if (this->ws_listen() == false) return nullptr;
  } else {
    this->listen_pending = false;
  }

  //std::cout << "IOSocket::accept() calling ws_accept" << std::endl;
  SOCKET handle = this->ws_accept();
  if (this->ws_is_valid_socket(handle)) {
    IOSocket* client = new IOSocket();
    client->handle = handle;
    client->ws_set_nonblocking();
    client->closed = false;
    client->protocol = "tcp";
    //std::cout << "IOSocket" << this << "::accept() received a valid socket " << client << std::endl;
    return client;
  } else {
    return nullptr;
  }
}


// Public static factory method
IOSocket* IOSocket::connect(std::string host, std::string port, std::string protocol)
{
  IOSocket* socket = new IOSocket();
  socket->peer_host = host;
  socket->peer_port = port;
  socket->protocol = protocol;
  socket->error = -1;

  if (socket->ws_resolve_address(host, port) == false) return socket;
  socket->peer_addr = socket->resolved[0];

  if (protocol == "tcp" && socket->ws_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) == false) return socket;
  if (protocol == "udp" && socket->ws_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) == false) return socket;
  if (protocol != "tcp" && protocol != "udp") { socket->error = -1; return socket; }

  if (socket->ws_set_nonblocking() == false) return socket;
  if (socket->ws_set_keepalive() == false) return socket;
  if (socket->ws_set_reuseaddr() == false) return socket;
  if (socket->ws_connect() == false) return socket;

  // Note: The socket may not actually be connected yet because we set it to
  // non-blocking mode, but atleast we know it was not immediately declined.
  // Subsequent send() and recv() operations may return transient failures.
  // We use ws_is_transient() to test for these.

  socket->error = 0;
  socket->closed = false;
  //std::cout << "IOSocket::connect() ok" << std::endl;
  return socket;
}


std::string IOSocket::read(const int bytes)
{
  //std::cout << "IOSocket" << this << "::read() called (rq=" << this->r_buffer.size() << ",wq=" << this->w_buffer.size() << ")" << std::endl;
  if (this->protocol == "udp") return "";

  flush_w_buffer(); // Try to send pending data

  int read_bytes = bytes;
  //std::cout << "IOSocket::read() bytes=" << bytes << std::endl;
  if (read_bytes <= 0) read_bytes = this->bufsize;
  if ((int) r_buffer.size() < this->bufsize) fill_r_buffer();
  std::string result = drain_r_buffer(bytes); // Get line from buffer
  //std::cout << "IOSocket" << this << "::read() drained buffer: '" << result << "'" << std::endl;
  return result;
}

std::string IOSocket::readln()
{
  if (this->protocol == "udp") return "";
  std::string result;
  std::vector<char>::iterator pos;

  flush_w_buffer(); // Try to send pending data

  pos = std::find(r_buffer.begin(), r_buffer.end(), readln_newline);

  if (pos == r_buffer.end() && this->closed == false) {
    //std::cout << "IOSocket" << this << "::readln() no newline in buffer, fill_buffer" << std::endl;
    fill_r_buffer();
    pos = std::find(r_buffer.begin(), r_buffer.end(), readln_newline);
  }
  //std::cout << "IOSocket::readln() pos=" << pos << std::endl;
  if (pos == r_buffer.end() && this->closed == false) {
    //std::cout << "IOSocket" << this << "::readln() no data available yet, returning empty string" << std::endl;
    return ""; // No more data available yet
  }

  // If we get this far it means EITHER we would a newline at 'pos',
  // OR the connection closed and 'pos' points to the end of the buffer.
  // Add 1 because distance() is an offset and we want the size.
  size_t bytes = std::distance(r_buffer.begin(), pos) + 1;
  result = drain_r_buffer(bytes); // Get line from buffer
  //std::cout << "IOSocket" << this << "::readln() drained buffer: '" << result << "'" << std::endl;
  return result;
}



int IOSocket::write(const std::string data)
{
  //std::cout << "IOSocket" << this << "::write() called (rq=" << this->r_buffer.size() << ",wq=" << this->w_buffer.size() << ")" << std::endl;
  if (this->protocol == "udp") return -1;
  if (this->closed) return -1;

  fill_w_buffer(data); // Enqueue data
  flush_w_buffer(); // Try to send pending data

  int bytes_written = data.length();
  //std::cout << "IOSocket" << this << "::write() returning " << bytes_written << std::endl;
  return bytes_written;
}


int IOSocket::writeln(const std::string data)
{
  //std::cout << "IOSocket" << this << "::writeln() called (rq=" << this->r_buffer.size() << ",wq=" << this->w_buffer.size() << ")" << std::endl;
  std::string line = data;
  int bytes_written = write(line.append(newline));
  //bytes_written += write(newline);
  //std::cout << "IOSocket::writeln() returning " << bytes_written << std::endl;
  return bytes_written;
}


std::string IOSocket::getc()
{
  //std::cout << "IOSocket::getc()" << std::endl;
  if ((int) r_buffer.size() < this->bufsize) fill_r_buffer();
  return drain_r_buffer(1);
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
  this->w_buffer.clear();
}

// Do not call directly, use close() instead
void IOSocket::close_udp()
{
  this->pw_buffer.clear();
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


void IOSocket::fill_r_buffer()
{
  if (this->closed) return;

  //std::cout << "IOSocket" << this << "::fill_tcp_buffer() calling ws_recv (rq=" << r_buffer.size() << ")" << std::endl;
  std::vector<char> tmpbuf = ws_recv();
  if (tmpbuf.size() > 0) {
    std::copy(tmpbuf.begin(), tmpbuf.end(), std::back_inserter(r_buffer));
  }
  //std::cout << "IOSocket" << this << "::fill_tcp_buffer() received " << tmpbuf.size() << " bytes (rq=" << r_buffer.size() << ")" << std::endl;
}

std::string IOSocket::drain_r_buffer(const size_t bytes)
{
  //std::cout << "IOSocket" << this << "::drain_buffer() bytes=" << bytes << std::endl;
  size_t want_bytes = bytes;
  if (want_bytes > r_buffer.size()) want_bytes = r_buffer.size();

  std::string result(r_buffer.begin(), r_buffer.begin() + want_bytes);
  r_buffer.erase(r_buffer.begin(), r_buffer.begin() + want_bytes);
  //std::cout << "IOSocket" << this << "::drain_buffer() returning '" << result << "'" << std::endl;
  return result;
}

void IOSocket::fill_w_buffer(const std::string data)
{
  if (this->closed) return;
  std::copy(data.begin(), data.end(), std::back_inserter(this->w_buffer));
}

void IOSocket::flush_w_buffer()
{
  //std::cout << "IOSocket" << this << "::flush_tcp_buffer() called" << std::endl;
  if (this->closed) return;

  while (this->w_buffer.size() > 0) {
    size_t bytes_to_write = this->w_buffer.size();
    if ((int) bytes_to_write > this->bufsize) bytes_to_write = this->bufsize;
    //std::cout << "IOSocket" << this << "::flush_tcp_buffer() try to write " << bytes_to_write << " bytes" << std::endl;
    std::vector<char> data(w_buffer.begin(), w_buffer.begin() + bytes_to_write);

    size_t bytes_written = ws_send(data);
    //std::cout << "IOSocket" << this << "::flush_tcp_buffer() wrote " << bytes_written << " bytes" << std::endl;
    if (bytes_written == 0) return;
    w_buffer.erase(w_buffer.begin(), w_buffer.begin() + bytes_written);
  }

  return;
}


// ==== winsock specific methods ====
// https://docs.microsoft.com/nb-no/windows/win32/api/winsock





bool IOSocket::ws_resolve_address(std::string hostname, std::string port)
{
  bool result = false;

  if (this->resolved != nullptr) {
    freeaddrinfo(this->resolved);
    this->resolved = nullptr;
  }
  struct addrinfo hints = {0};
  hints.ai_family = AF_INET;

  int status = getaddrinfo(hostname.c_str(), port.c_str(), &hints, &this->resolved);

  if (status == 0 && this->resolved != nullptr) {
    result = true;
    //std::cout << "IOSocket::ws_resolve_address() getaddrinfo ok (code=" << code << ")" << std::endl;
  } else {
    return ws_check_error("resolve_address");
  }

  return result;
}


bool IOSocket::ws_socket(int af_family, int sock_type, int ip_proto)
{
  this->handle = ::socket(af_family, sock_type, ip_proto);

  if (this->handle == INVALID_SOCKET) {
    ws_check_error("socket");
    return false;
  }

  //std::cout << "IOSocket::ws_socket() ok" << std::endl;
  return true;
}


bool IOSocket::ws_is_valid_socket(SOCKET handle)
{
  return (handle != INVALID_SOCKET);
}


SOCKET IOSocket::ws_accept()
{
  SOCKET client = ::accept(this->handle, NULL, NULL);
  if (client == INVALID_SOCKET) ws_check_error("accept");
  return client;
}


bool IOSocket::ws_set_nonblocking()
{
  //std::cout << "IOSocket::ws_set_nonblocking()" << std::endl;
  u_long mode = 1;  // 1 to enable non-blocking socket
  ioctlsocket(this->handle, FIONBIO, &mode);
  return true;
}


bool IOSocket::ws_set_keepalive()
{
  //std::cout << "IOSocket::ws_set_nonblocking()" << std::endl;
  bool value = true;
  int status = setsockopt(this->handle, SOL_SOCKET, SO_KEEPALIVE, (char*) &value, sizeof(bool));
  return ws_check_status("set_keepalive", status);
}


bool IOSocket::ws_set_reuseaddr()
{
  //std::cout << "IOSocket::ws_set_nonblocking()" << std::endl;
  bool value = true;
  int status = setsockopt(this->handle, SOL_SOCKET, SO_REUSEADDR, (char*) &value, sizeof(bool));
  return ws_check_status("set_keepalive", status);
}


bool IOSocket::ws_check_error(std::string context)
{
  this->error = WSAGetLastError();

  if (ws_is_transient(this->error)) {
    std::cout << "IOSocket" << this << "::ws_" << context << "() in progress" << std::endl;
    this->error = 0;
    return true;
  }

  if (ws_is_permanent(this->error)) {
    std::cerr << "IOSocket" << this << "::ws_" << context << "() failed: " << ws_errormessage(this->error) << std::endl;
    this->close();
    return false;
  }

  std::cerr << "IOSocket" << this << "::ws_" << context << "() unknown code: " << this->error << std::endl;
  this->close();
  return false;
}


bool IOSocket::ws_check_status(std::string context, int status)
{
  if (status != SOCKET_ERROR) {
    std::cout << "IOSocket" << this << "::ws_" << context << "()  ok" << std::endl;
    return true;
  } else {
    return ws_check_error(context);
  }
}


bool IOSocket::ws_connect()
{
  int status = ::connect(this->handle, this->peer_addr.ai_addr, (int) this->peer_addr.ai_addrlen);
  return ws_check_status("connect", status);
}


bool IOSocket::ws_bind()
{
  int status = ::bind(this->handle, this->host_addr.ai_addr, (int) this->host_addr.ai_addrlen);
  // This function must return false on transient failure
  if (WSAGetLastError() == WSAEINPROGRESS) return false;
  return ws_check_status("bind", status);
}


bool IOSocket::ws_listen()
{
  int status = ::listen(this->handle, SOMAXCONN);
  // This function must return false on transient failure
  if (WSAGetLastError() == WSAEINPROGRESS) return false;
  return ws_check_status("listen", status);
}




size_t IOSocket::ws_send(std::vector<char> buffer)
{
  int status = ::send(this->handle, buffer.data(), buffer.size(), 0);

  if (status == SOCKET_ERROR) {
    ws_check_status("send", status);
    return 0;
  }

  size_t bytes_sent = status;
  return bytes_sent;
}


std::vector<char> IOSocket::ws_recv()
{
  std::vector<char> buffer(this->bufsize, 0);
  int status = ::recv(this->handle, buffer.data(), buffer.size(), 0);

  if (status == SOCKET_ERROR) {
    ws_check_status("recv", status);
    buffer.clear();
    return buffer;
  }

  size_t bytes_received = status;
  buffer.resize(bytes_received);
  std::cout << "IOSocket::ws_recv() received " << bytes_received << " bytes ok" << std::endl;
  return buffer;
}


// Do not call directly, use close() instead
bool IOSocket::ws_close()
{
  //std::cout << "IOSocket::ws_close()" << std::endl;
  if (this->handle == INVALID_SOCKET) return false;
  ::closesocket(this->handle);
  this->handle = INVALID_SOCKET;
  return true;
}

bool IOSocket::ws_is_transient(int error)
{
  if (error == WSAEINPROGRESS)        return true; // Call is in progress
  if (error == WSAEALREADY)           return true; // A non-blocking connect has not completed yet
  if (error == WSAEWOULDBLOCK)        return true; // Not ready, try again
  if (error == WSATRY_AGAIN)          return true; // Try again
  if (error == WSAENOTCONN)           return true; // Not connected
  return false;
}

bool IOSocket::ws_is_permanent(int error)
{
  if (error == WSANOTINITIALISED)     return true; // WSAStartup failed or not called
  if (error == WSAENETDOWN)           return true; // The network subsystem has failed
  if (error == WSAENETRESET)          return true; // Connection reset
  if (error == WSAEADDRINUSE)         return true; // Address already in use
  if (error == WSAEINTR)              return true; // Cancelled with WSACancelBlockingCall
  if (error == WSAEADDRNOTAVAIL)      return true; // Remote address is invalid
  if (error == WSAEAFNOSUPPORT)       return true; // Invalid address for this address family
  if (error == WSAECONNREFUSED)       return true; // Connection refused
  if (error == WSAEFAULT)             return true; // Invalid socket address
  if (error == WSAEINVAL)             return true; // Socket is in listen mode
  if (error == WSAEISCONN)            return true; // Socket already connected
  if (error == WSAENETUNREACH)        return true; // Network unreachable
  if (error == WSAEHOSTUNREACH)       return true; // Host unreachable
  if (error == WSAENOBUFS)            return true; // No buffer space
  if (error == WSAENOTSOCK)           return true; // Not a socket
  if (error == WSAETIMEDOUT)          return true; // Connection timeout
  if (error == WSAEACCES)             return true; // Broadcast not enabled
  if (error == WSA_NOT_ENOUGH_MEMORY) return true; //
  if (error == WSAESOCKTNOSUPPORT)    return true; //
  if (error == WSAHOST_NOT_FOUND)     return true; //
  if (error == WSANO_DATA)            return true; //
  if (error == WSANO_RECOVERY)        return true; // A nonrecoverable error occurred (thank you, Windows)
  if (error == WSATYPE_NOT_FOUND)     return true; //
  if (error == WSAESHUTDOWN)          return true; // Connection shut down
  if (error == WSAECONNABORTED)       return true; // Connection aborted
  if (error == WSAECONNRESET)         return true; // Connection reset
  if (error == WSAETIMEDOUT)          return true; // Connection timeout
  return false;
}

std::string IOSocket::ws_errormessage(int error)
{
  // Transient errors
  if (error == WSAEINPROGRESS)        return "WSAEINPROGRESS";
  if (error == WSAEALREADY)           return "WSAEALREADY";
  if (error == WSAEWOULDBLOCK)        return "WSAEWOULDBLOCK";
  if (error == WSATRY_AGAIN)          return "WSATRY_AGAIN";
  if (error == WSAENOTCONN)           return "WSAENOTCONN";

  // Permanent errors
  if (error == WSAEACCES)             return "WSAEACCES";
  if (error == WSAENETDOWN)           return "WSAENETDOWN";
  if (error == WSAEADDRINUSE)         return "WSAEADDRINUSE";
  if (error == WSAEADDRNOTAVAIL)      return "WSAEADDRNOTAVAIL";
  if (error == WSAEAFNOSUPPORT)       return "WSAEAFNOSUPPORT";
  if (error == WSAECONNREFUSED)       return "WSAECONNREFUSED";
  if (error == WSAECONNABORTED)       return "WSAECONNABORTED";
  if (error == WSAECONNRESET)         return "WSAECONNRESET";
  if (error == WSAEFAULT)             return "WSAEFAULT";
  if (error == WSAEINTR)              return "WSAEINTR";
  if (error == WSAEISCONN)            return "WSAEISCONN";
  if (error == WSAEINVAL)             return "WSAEINVAL";
  if (error == WSAENETUNREACH)        return "WSAENETUNREACH";
  if (error == WSAENETRESET)          return "WSAENETRESET";
  if (error == WSAEHOSTUNREACH)       return "WSAEHOSTUNREACH";
  if (error == WSAENOBUFS)            return "WSAENOBUFS";
  if (error == WSAENOTSOCK)           return "WSAENOTSOCK";
  if (error == WSAETIMEDOUT)          return "WSAETIMEDOUT";
  if (error == WSANOTINITIALISED)     return "WSANOTINITIALISED";
  if (error == WSA_NOT_ENOUGH_MEMORY) return "WSA_NOT_ENOUGH_MEMORY";
  if (error == WSANO_DATA)            return "WSANO_DATA";
  if (error == WSANO_RECOVERY)        return "WSANO_RECOVERY";
  if (error == WSAESOCKTNOSUPPORT)    return "WSAESOCKTNOSUPPORT";
  if (error == WSAHOST_NOT_FOUND)     return "WSAHOST_NOT_FOUND";
  if (error == WSATYPE_NOT_FOUND)     return "WSATYPE_NOT_FOUND";
  if (error == WSAESHUTDOWN)          return "WSAESHUTDOWN";
  if (error == WSAETIMEDOUT)          return "WSAETIMEDOUT";

  return std::string("UNKNOWN=").append(std::to_string(error));
}
