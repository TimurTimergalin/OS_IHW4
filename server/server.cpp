#include <arpa/inet.h>
#include <stdexcept>
#include <cstring>

#include "server.h"
#include "../constants.h"
#include "handler.h"
#include "../inet_utils.h"

namespace server {
    int connect(unsigned short port) {
        sockaddr_in info{};
        int sock = inet::udp_socket();
        inet::address(port, info);

        inet::bind(sock, info);

        return sock;
    }

    void work(int sock, Out &out) {
        out.write("Work started\n");
        sockaddr_in info{};

        for (;;) {
            std::string msg = inet::recv(sock, info);
            out.write("Message received\n");
            handle_message(msg, info, out, sock);
        }

    }

    void handle(unsigned short port, Out &out) {
        int in_sock = connect(port);
        work(in_sock, out);
    }
}
