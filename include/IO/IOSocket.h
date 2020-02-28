#ifndef IOSOCKET_H
#define IOSOCKET_H

#include "IO/IOHandle.h"

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
    std::vector<std::vector<char>> pr_buffer; // FIXME
    std::vector<std::vector<char>> pw_buffer; // FIXME
    bool listen_pending = false;

    std::string peer_host;
    std::string peer_port;
    std::string protocol;

//    bool is_ready();
    void fill_r_buffer(); // Receive into r_buffer (bytes)
    void fill_pr_buffer(); // Receive into pr_buffer (packets)
    std::string drain_r_buffer(const size_t bytes); // Take bytes from r_buffer
    void fill_w_buffer(const std::string data); // Add data into w_buffer (bytes)
    void flush_w_buffer(); // Send data from w_buffer (bytes)
    void flush_pw_buffer(); // Send data from pw_buffer (packets)
    void close_tcp();
    void close_udp();

    // Winsock specific variables
    addrinfo peer_addr = { 0 };
    addrinfo host_addr = { 0 };
    addrinfo* resolved = nullptr;
    SOCKET handle = INVALID_SOCKET;

    // Winsock specific methods
    bool ws_resolve_address(std::string hostname, std::string port);
    bool ws_socket(int af_family, int sock_type, int ip_proto);
    bool ws_set_nonblocking();
    bool ws_set_keepalive();
    bool ws_set_reuseaddr();
    bool ws_check_error(std::string context);
    bool ws_check_status(std::string context, int status);
    bool ws_is_valid_socket(SOCKET);
    SOCKET ws_accept();
    bool ws_connect();
    bool ws_bind();
    bool ws_listen();
    bool ws_close();
    std::vector<char> ws_recv();
    size_t ws_send(std::vector<char> buffer);
    bool ws_is_transient(int error);
    bool ws_is_permanent(int error);
    std::string ws_errormessage(int error);

};




#endif // IOSOCKET_H
