#ifndef OS_IDZ4_HANDLER_H
#define OS_IDZ4_HANDLER_H

#include "../out/Out.h"

namespace server {
    void handle_message(const std::string &message, const sockaddr_in &info, Out &out, int out_sock);
    void init();
}

#endif //OS_IDZ4_HANDLER_H
