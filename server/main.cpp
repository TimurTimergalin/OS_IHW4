#include <string>
#include <stdexcept>
#include <sstream>
#include <memory>
#include <csignal>

#include "../out/Out.h"
#include "server.h"
#include "../parse_short.h"
#include "handler.h"

std::unique_ptr<Out> out;
std::string header = "Server:";

void handle_exit(int s) {
    out->write("finish");
    if (s == SIGABRT) {
        abort();
    } else {
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2 && argc != 4 && argc != 5) {
        std::stringstream ss{};
        ss << "Usage: " << argv[0] << " server-port [output-ip output-port [-b]]";
        throw std::runtime_error(ss.str());
    }

    unsigned short self_port = parse_short(argv[1]);
    if (argc == 2) {
        out = std::make_unique<StreamOut>(std::cout, header);
    } else if (argc == 4) {
        std::string ip = argv[2];
        unsigned short out_port = parse_short(argv[3]);
        out = std::make_unique<UdpOut>(ip, out_port, false, header);
    } else if (std::string(argv[4]) != "-b") {
        throw std::runtime_error("Invalid option");
    } else {
        std::string ip = argv[2];
        unsigned short out_port = parse_short(argv[3]);
        out = std::make_unique<UdpOut>(ip, out_port, true, header);
    }

    signal(SIGINT, handle_exit);
    signal(SIGTERM, handle_exit);
    signal(SIGQUIT, handle_exit);
    signal(SIGABRT, handle_exit);

    server::init();
    server::handle(self_port, *out);
}