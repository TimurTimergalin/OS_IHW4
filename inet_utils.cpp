#include <stdexcept>
#include <cstring>

#include "constants.h"
#include "inet_utils.h"

int inet::udp_socket() {
    int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        throw std::runtime_error("Unable to open socket");
    }
    return sock;
}

void inet::address(const std::string &ip, unsigned short port, sockaddr_in &info) {
    memset(&info, 0, sizeof(info));
    info.sin_family = AF_INET;
    info.sin_port = htons(port);
    if (inet_aton(ip.c_str(), &info.sin_addr) < 0) {
        throw std::runtime_error("Invalid ip address");
    }
}

void inet::address(unsigned short port, sockaddr_in &info) {
    memset(&info, 0, sizeof(info));
    info.sin_family = AF_INET;
    info.sin_port = htons(port);
    info.sin_addr.s_addr = htonl(INADDR_ANY);
}

void inet::enable_broadcast(int sock) {
    int perm = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &perm, sizeof(perm)) < 0) {
        throw std::runtime_error("Unable to broadcast on this ip");
    }
}

void inet::bind(int sock, const sockaddr_in &info) {
    auto bind_res = bind(
            sock,
            reinterpret_cast<const sockaddr *>(&info),
            sizeof(info)
    );
    if (bind_res < 0) {
        throw std::runtime_error("Unable to bind socket");
    }
}

void inet::send(int sock, const std::string &msg, const sockaddr_in &info) {
    ssize_t sent = sendto(
            sock,
            msg.c_str(),
            msg.size(),
            0,
            reinterpret_cast<const sockaddr *>(&info),
            sizeof(info));

    if (sent != msg.size()) {
        throw std::runtime_error("Unable to send the whole message");
    }
}

std::string inet::recv(int sock, sockaddr_in &info) {
    socklen_t len = sizeof(info);
    char msg_buf[message_max_size];

    ssize_t res = recvfrom(
            sock,
            msg_buf,
            message_max_size,
            0,
            reinterpret_cast<sockaddr *>(&info),
            &len
    );

    if (len != sizeof(info)) {
        throw std::runtime_error("Unable to extract receive address");
    }

    if (res <= 0) {
        throw std::runtime_error("Unable to receive a message");
    }
    std::string msg = std::string(msg_buf, res);

    return msg;
}


