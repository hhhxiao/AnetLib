//
// Created by hhhxiao on 5/11/20.
//

#ifndef ANET_TCPCONNECTION_H
#define ANET_TCPCONNECTION_H

#include <utility>
#include <sys/epoll.h>

#include "IOListener.h"

#define MAX_BUFF_SIZE 1024

class TcpConnection {
private:
    int socket;
    IOListener *listener;
    std::string inputBuffer;
    std::function<void()> readEvent;
    std::function<void()> closeEvent;
public:
    void sendMessage(const char *msg) const;

    ~TcpConnection();

    TcpConnection(const TcpConnection &) = delete;

    TcpConnection &operator=(const TcpConnection &) = delete;


    explicit TcpConnection(int socket);

    void onRead(std::function<void()> todo);

    void onClose(std::function<void()> todo);

    void onWrite() {

    }

    inline void close() const { ::close(this->socket); }

    inline IOListener *getListener() {
        return this->listener;
    }

    std::string readAll() const {
        int len = 0;
        std::string s;
        char buffer[MAX_BUFF_SIZE];
        memset(buffer, 0, MAX_BUFF_SIZE);
        do {
            len = ::read(this->socket, buffer, MAX_BUFF_SIZE);
            // printf("read:%s", buffer);
            s += std::string(buffer);
            buffer[0] = 0;
        } while (len > 0);
        return s;
    }
};

TcpConnection::TcpConnection(int socket) : socket(socket) {
    this->listener = new IOListener(socket, EPOLLIN | EPOLLET);
}

void TcpConnection::onRead(std::function<void()> todo) {
    this->readEvent = std::move(todo);
    this->listener->onRead([this]() {
        this->readEvent();
    });
}

void TcpConnection::onClose(std::function<void()> todo) {
    this->closeEvent = std::move(todo);
}

TcpConnection::~TcpConnection() {
    delete this->listener;
}

void TcpConnection::sendMessage(const char *msg) const {
    printf("current fd:%d\n", this->socket);
    ::write(this->socket, msg, strlen(msg));
}


#endif //ANET_TCPCONNECTION_H
