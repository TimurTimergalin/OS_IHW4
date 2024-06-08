#include <stdexcept>
#include <arpa/inet.h>
#include <random>
#include <sstream>
#include <thread>
#include <chrono>

#include "../out/Out.h"
#include "../constants.h"
#include "../inet_utils.h"
#include "../parse_short.h"

namespace producer {
    std::mt19937 rg(std::random_device{}());
    int sleep_time = std::uniform_int_distribution<int>{500, 5000}(rg);

    std::unique_ptr<Out> pout;
    int connect() {
        return inet::udp_socket();
    }

    void work(const std::string &ip, unsigned short port, int sock) {
        Out& out = *pout;
        sockaddr_in info{};
        inet::address(ip, port, info);

        std::uniform_int_distribution<int> res_dist{0, resource_count - 1};

        out.write("Starting working\n");
        for (;;) {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
            int res = res_dist(rg);
            {
                std::stringstream ss{};
                ss << "Producing all resources except " << res << "\n";
                out.write(ss.str());
            }

            std::stringstream ss{};
            ss << "resources ";
            for (int i = 0; i < resource_count; ++i) {
                if (i == res) {
                    ss << 1;
                } else {
                    ss << 0;
                }
                if (i + 1 < resource_count) {
                    ss << " ";
                }
            }

            inet::send(sock, ss.str(), info);
            out.write("Delivery made\n");
        }
    }

    struct WorkArgs {
        std::string ip;
        unsigned short port;
        int sock;
    };

    void *work(void *args) {
        auto &[ip, port, sock] = *reinterpret_cast<WorkArgs *>(args);
        work(ip, port, sock);
        return nullptr;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3 && argc != 5 && argc != 6) {
        std::stringstream ss{};
        ss << "Usage " << argv[0] << " server-ip server-port [out-ip out-port [-b]]";
        throw std::runtime_error(ss.str());
    }

    unsigned short server_port = parse_short(argv[2]);

    std::string header = "Producer:";

    pthread_t worker;
    if (argc == 3) {
        producer::pout = std::make_unique<StreamOut>(std::cout, header);
        producer::WorkArgs args{argv[1], server_port, producer::connect()};
        pthread_create(&worker, nullptr, producer::work, &args);
    } else {
        unsigned short out_port = parse_short(argv[4]);
        producer::pout = std::make_unique<UdpOut>(argv[3], out_port, (argc == 6 && std::string(argv[5]) == "-b"), header);
        producer::WorkArgs args{argv[1], server_port, producer::connect()};
        pthread_create(&worker, nullptr, producer::work, &args);
    }

    pthread_join(worker, nullptr);
    return 0;
}