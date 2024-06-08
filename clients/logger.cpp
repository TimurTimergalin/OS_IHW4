#include <string>
#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include "../inet_utils.h"
#include "../parse_short.h"

namespace logger {
    void work(int sock, std::ostream &out) {
        std::string msg;

        sockaddr_in info{};

        auto step = [&]() {
            msg = inet::recv(sock, info);
            out << msg;
        };

        step();
        while (msg != "Server: finish") {
            step();
        }
    }

    int connect(unsigned short port, bool broadcast) {
        int sock = inet::udp_socket();
        if (broadcast) {
            inet::enable_broadcast(sock);
        }
        sockaddr_in info{};
        inet::address(port, info);
        inet::bind(sock, info);

        return sock;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3 && argc != 4) {
        std::stringstream ss{};
        ss << "Usage " << argv[0] << " port (b|n) [filename]";
        throw std::runtime_error(ss.str());
    }

    unsigned short port = parse_short(argv[1]);
    int sock = logger::connect(port, std::string(argv[2]) == "b");
    if (argc == 3) {
        logger::work(sock, std::cout);
    } else {
        std::ofstream out(argv[3]);
        logger::work(sock, out);
    }
    return 0;
}

