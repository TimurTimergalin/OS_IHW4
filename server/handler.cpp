#include "handler.h"
#include "../constants.h"
#include "../inet_utils.h"

#include <sstream>
#include <cstring>
#include <queue>
#include <array>

struct Receiver {
    int resource;
    sockaddr_in info{};

    Receiver(int resource, const sockaddr_in &info) : resource(resource) {
        memcpy(&this->info, &info, sizeof(info));
    }

    Receiver(const Receiver &other) {
        resource = other.resource;
        memcpy(&info, &other.info, sizeof(info));
    }
};

std::queue<Receiver> rq{};
std::array<int, resource_count> resources{};

namespace server {
    void init() {
        resources.fill(0);
    }

    void serve(int out_sock, Out &out) {
        std::queue<Receiver> new_q{};

        while (!rq.empty()) {
            Receiver top(rq.front());
            rq.pop();
            bool ready = true;
            for (int i = 0; i < resource_count; ++i) {
                if (i != top.resource && resources[i] == 0) {
                    ready = false;
                    break;
                }
            }
            if (!ready) {
                new_q.push(top);
            } else {
                std::stringstream ss{};
                ss << "Smoker with resource " << top.resource << " is being served\n";
                out.write(ss.str());

                std::string msg = "granted";

                for (int i = 0; i < resource_count; ++i) {
                    if (i != top.resource) {
                        --resources[i];
                    }
                }

                inet::send(out_sock, msg, top.info);
            }
        }
        rq = std::move(new_q);
    }

    void handle_message(const std::string &message, const sockaddr_in &info, Out &out, int out_sock) {
        std::stringstream ss{};
        ss << message;
        std::string type;
        ss >> type;

        bool perform_serve = true;
        if (type == "request") {
            int res;
            ss >> res;
            if (res < 0 || res >= resource_count) {
                out.write("Received invalid resource request. Ignoring...\n");
                perform_serve = false;
            }
            std::stringstream ss1{};
            ss1 << "Smoker with resource " << res << " requested resources\n";
            out.write(ss1.str());
            rq.emplace(res, info);
        } else if (type == "resources" || type == "return") {
            std::array<int, resource_count> to_add{};
            for (int i = 0; i < resource_count; ++i) {
                int count;
                ss >> count;
                if (count < 0) {
                    out.write("Resource delivery with invalid resource count. Ignoring...\n");
                    perform_serve = false;
                    break;
                }
                to_add[i] = count;
            }
            if (perform_serve) {
                for (int i = 0; i < resource_count; ++i) {
                    resources[i] += to_add[i];
                }
            }
            if (type == "resources") {
                out.write("Resources delivered from producer\n");
            } else {
                out.write("Resource returned by the smoker\n");
            }
        } else {
            out.write("Unknown request type\n");
            perform_serve = false;
        }

        if (perform_serve) {
            serve(out_sock, out);
        }
    }
}
