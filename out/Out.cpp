#include "Out.h"

Out::~Out() = default;

Out::Out(std::string header) : header(std::move(header)){

}

void Out::write(const std::string &message) {
    _write(header + " " + message);
}
