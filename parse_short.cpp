#include <string>
#include <limits>
#include <stdexcept>

#include "parse_short.h"

unsigned short parse_short(const char *port_s) {
    int iport = std::stoi(port_s);
    if (iport > std::numeric_limits<unsigned short>::max() ||
        iport < std::numeric_limits<unsigned short>::min()) {
        throw std::runtime_error("Invalid port value");
    }
    return static_cast<unsigned short>(iport);
}