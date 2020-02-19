#ifndef IOSOCKET_H
#define IOSOCKET_H

#include "IOHandle.h"

#include "SDL_net.h"

class IOSocket : public IOHandle
{
  public:
    IOSocket();
    ~IOSocket();

    bool is_eof();

    static IOSocket* listen(uint16_t, std::string protocol);
    static IOSocket* connect(std::string host, uint16_t port, std::string protocol);
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
    std::vector<UDPpacket> pr_buffer;
    std::vector<UDPpacket> pw_buffer;

    IPaddress peer_inetaddr; // ip+port
    TCPsocket tcp_sock;
    UDPsocket udp_sock;
    SDLNet_SocketSet sock_set;
    std::string peer_host;
    uint16_t peer_port;
    std::string protocol;

    bool is_ready();
    void fill_tcp_buffer(); // Attempt to receive into r_buffer (bytes)
    void fill_udp_buffer(); // Attempt to receive into pr_buffer (packets)
    std::string drain_buffer(const size_t bytes); // Take bytes from r_buffer
    size_t flush_tcp_buffer(size_t bytes); // Attempt to send data from w_buffer (bytes)
    void close_tcp();
    void close_udp();

};

#endif // IOSOCKET_H
