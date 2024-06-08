#ifndef OS_IDZ4_OUT_H
#define OS_IDZ4_OUT_H

#include <iostream>
#include <string>
#include <pthread.h>

#include <arpa/inet.h>

class Out {
    std::string header;
public:
    void write(const std::string &message);
    virtual ~Out();
protected:
    explicit Out(std::string header);
    virtual void _write(const std::string &message) = 0;
};

class StreamOut : public Out {
    std::ostream &os;
    pthread_mutex_t mut{};
public:
    explicit StreamOut(std::ostream &, std::string s);
    ~StreamOut() override;
protected:
    void _write(const std::string &message) override;
};

class UdpOut : public Out {
    int sock;
    sockaddr_in info;

public:
    UdpOut(const std::string &ip, unsigned short port, bool broadcast, std::string s);
    ~UdpOut() override;
protected:
    void _write(const std::string &message) override ;
};



#endif //OS_IDZ4_OUT_H
