#ifndef OS_IDZ4_INET_UTILS_H
#define OS_IDZ4_INET_UTILS_H

#include <arpa/inet.h>
#include <string>

namespace inet {
    int udp_socket();

    void address(const std::string &ip, unsigned short port, sockaddr_in &info);
    void address(unsigned short port, sockaddr_in &info);

    void enable_broadcast(int sock);

    void bind(int sock, const sockaddr_in &info);

    void send(int sock, const std::string &msg, const sockaddr_in &info);

    std::string recv(int sock, sockaddr_in &info);
}

#endif //OS_IDZ4_INET_UTILS_H
