#include <string>
#include <pthread.h>
#include <random>
#include <thread>
#include <chrono>
#include <memory>
#include <sstream>

#include "../inet_utils.h"
#include "../out/Out.h"
#include "../constants.h"
#include "../parse_short.h"

namespace smoker {
    std::unique_ptr<Out> out;
    int connect(unsigned short port) {
        int sock = inet::udp_socket();
        sockaddr_in info{};
        inet::address(port, info);
        inet::bind(sock, info);
        return sock;
    }

    bool smoking = false;

    struct AutoMutex {
        pthread_mutex_t mutex{};
        AutoMutex()  {
            pthread_mutex_init(&mutex, nullptr);
        }
        ~AutoMutex() {
            pthread_mutex_destroy(&mutex);
        }
    } m;

    struct AutoCond {
        pthread_cond_t cond{};
        AutoCond() {
            pthread_cond_init(&cond, nullptr);
        }

        ~AutoCond() {
            pthread_cond_destroy(&cond);
        }
    } c;

    struct WorkArgs {
        std::string ip;
        unsigned short port;
        int sock;
        int res;
    };

    void *smoke(void *pout) {
        auto &[ip, port, sock, res] = *reinterpret_cast<WorkArgs *>(pout);
        sockaddr_in info{};
        inet::address(ip, port, info);

        auto send_request = [&]() {
            std::stringstream ss{};
            ss << "request " << res;
            inet::send(sock, ss.str(), info);
        };

        static std::mt19937 rg(std::random_device{}());
        static int smoke_time = std::uniform_int_distribution<int>{500, 5000}(rg);
        send_request();
        for (;;) {
            pthread_mutex_lock(&m.mutex);
            while (!smoking) {
                pthread_cond_wait(&c.cond, &m.mutex);
            }
            out->write("Starting smoking\n");
            pthread_mutex_unlock(&m.mutex);
            std::this_thread::sleep_for(std::chrono::milliseconds(smoke_time));

            pthread_mutex_lock(&m.mutex);
            smoking = false;
            pthread_mutex_unlock(&m.mutex);
            out->write("Finishing smoking\n");
            send_request();
        }
    }

    void *communicate(void *args) {
        auto &[ip, port, sock, res] = *reinterpret_cast<WorkArgs *>(args);
        sockaddr_in info{};
        inet::address(ip, port, info);

        out->write("Starting\n");

        for (;;) {
            std::string msg = inet::recv(sock, info);
            if (msg != "granted") {
                out->write("Invalid message. Ignoring...\n");
                continue;
            }
            pthread_mutex_lock(&m.mutex);
            if (smoking) {
                std::stringstream ss{};
                ss << "return ";
                for (int i = 0; i < resource_count; ++i) {
                    if (i == res) {
                        ss << 0;
                    } else {
                        ss << 1;
                    }
                    ss << ' ';
                }
                inet::send(sock, ss.str(), info);
            } else {
                smoking = true;
                pthread_cond_signal(&c.cond);
            }
            pthread_mutex_unlock(&m.mutex);
        }
    }

    void *work(void *args) {
        pthread_t smoker;
        pthread_t communicator;

        pthread_create(&smoker, nullptr, smoke, args);
        pthread_create(&communicator, nullptr, communicate, args);

        pthread_join(smoker, nullptr);
        pthread_join(communicator, nullptr);
        return nullptr;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 5 && argc != 7 && argc != 8) {
        std::stringstream ss{};
        ss << "Usage: " << argv[0] << " resource port server-ip server-port [out-ip out-port [-b]]";
        throw std::runtime_error(ss.str());
    }

    int res = std::stoi(argv[1]);
    std::string header;
    {
        std::stringstream ss{};
        ss << "Smoker " << res << ':';
        header = ss.str();
    }

    unsigned short port = parse_short(argv[2]);
    unsigned short server_port = parse_short(argv[4]);

    pthread_t worker;
    if (argc == 5) {
        smoker::out = std::make_unique<StreamOut>(std::cout, header);
        smoker::WorkArgs args{argv[3], server_port, smoker::connect(port), res};
        pthread_create(&worker, nullptr, smoker::work, &args);
    } else {
        unsigned short out_port = parse_short(argv[6]);
        smoker::out = std::make_unique<UdpOut>(argv[5], out_port, (argc == 8 && std::string(argv[7]) == "-b"), header);
        smoker::WorkArgs args{argv[3], server_port, smoker::connect(port), res};
        pthread_create(&worker, nullptr, smoker::work, &args);
    }

    pthread_join(worker, nullptr);
    return 0;
}