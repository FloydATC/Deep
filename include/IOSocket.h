#ifndef IOSOCKET_H
#define IOSOCKET_H

#include "IOHandle.h"

#include <ws2tcpip.h>


class IOSocket : public IOHandle
{
  public:
    IOSocket();
    ~IOSocket();

    bool is_eof();
    bool is_closed();

    static IOSocket* listen(std::string port, std::string protocol);
    static IOSocket* connect(std::string host, std::string port, std::string protocol);
    IOSocket* accept();
    std::string read(const int bytes);
    std::string readln();
    int write(const std::string data);
    int writeln(const std::string data);
    std::string getc();
    int send(std::string data);
    std::string recv();
    void close();

  protected:

  private:
    int bufsize = 1024; // Applies to TCP communication
    std::vector<char> pr_buffer; // FIXME
    std::vector<char> pw_buffer; // FIXME

    std::string peer_host;
    std::string peer_port;
    std::string protocol;

    bool is_ready();
    void fill_tcp_buffer(); // Attempt to receive into r_buffer (bytes)
    void fill_udp_buffer(); // Attempt to receive into pr_buffer (packets)
    std::string drain_buffer(const size_t bytes); // Take bytes from r_buffer
    size_t flush_tcp_buffer(size_t bytes); // Attempt to send data from w_buffer (bytes)
    void close_tcp();
    void close_udp();

    // Winsock specific variables
    WSADATA wsaData;
    struct addrinfo* peer_addr = nullptr;
    struct addrinfo host_addr;
    SOCKET handle = INVALID_SOCKET;

    // Winsock specific methods
    bool ws_initialize();
    bool ws_resolve_address(std::string hostname, std::string port);
    bool ws_create_tcp_socket();
    bool ws_create_udp_socket();
    bool ws_set_nonblocking();
    bool ws_connect();
    bool ws_close();
    std::string ws_recv();
    size_t ws_send(std::string data);
    bool ws_is_retry(int result);
    bool ws_is_error(int result);
    std::string ws_errormessage(int result);

};

#endif // IOSOCKET_H
