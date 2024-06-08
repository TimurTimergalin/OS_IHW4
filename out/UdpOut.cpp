#include <cstring>
#include <unistd.h>

#include "Out.h"
#include "../inet_utils.h"

UdpOut::UdpOut(const std::string &ip, unsigned short port, bool broadcast, std::string s) : Out(std::move(s)), sock(), info() {
    sock = inet::udp_socket();

    if (broadcast) {
        inet::enable_broadcast(sock);
    }

    inet::address(ip, port, info);
}

void UdpOut::_write(const std::string &message) {
    inet::send(sock, message, info);
}

UdpOut::~UdpOut() {
    close(sock);
}
