#include "Out.h"

StreamOut::StreamOut(std::ostream &os, std::string s): Out(std::move(s)), os(os) {
    pthread_mutex_init(&mut, nullptr);
}

void StreamOut::_write(const std::string &message) {
    pthread_mutex_lock(&mut);
    os << message << std::flush;
    pthread_mutex_unlock(&mut);
}

StreamOut::~StreamOut() {
    pthread_mutex_destroy(&mut);
}
