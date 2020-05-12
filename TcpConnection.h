//
// Created by hhhxiao on 5/11/20.
//

#ifndef ANET_TCPCONNECTION_H
#define ANET_TCPCONNECTION_H

#include <utility>
#include <sys/epoll.h>
#include <queue>
#include "IOListener.h"

#define MAX_BUFF_SIZE 64

/**
 * @author xiaohengying 2220/5/1
 * An abstract of Tcp connection
 */
class TcpConnection {
private:
    int socket;
    IOListener *listener;
    std::string inputBuffer;
    std::function<void()> readEvent;
    std::function<void()> closeEvent;

    void readAll();

public:
    void sendMessage(const char *msg) const;

    ~TcpConnection();

    //disable copy
    TcpConnection(const TcpConnection &) = delete;

    TcpConnection &operator=(const TcpConnection &) = delete;


    explicit TcpConnection(int socket);

    void onRead(std::function<void()> todo);

    void onClose(std::function<void()> todo);

    std::string getText() {
        std::string s(this->inputBuffer);
        this->inputBuffer.clear();
        return s;
    }

    void clearBuffer() {
        this->inputBuffer.clear();
    }


    inline void close() const { ::close(this->socket); }

    inline IOListener *getListener() {
        return this->listener;
    }
};


TcpConnection::TcpConnection(int socket) : socket(socket) {
    this->listener = new IOListener(socket, EPOLLIN | EPOLLET);
    this->listener->setReadEvent([this] {
        this->readAll();
        this->readEvent();
    });
}

void TcpConnection::onRead(std::function<void()> todo) {
    this->readEvent = std::move(todo);
}


void TcpConnection::onClose(std::function<void()> todo) {
    this->closeEvent = std::move(todo);
}

TcpConnection::~TcpConnection() {
    delete this->listener;
}

void TcpConnection::sendMessage(const char *msg) const {
    ::write(this->socket, msg, strlen(msg));
}

void TcpConnection::readAll() {
    int bytes_len;
    char buffer[MAX_BUFF_SIZE];
    memset(buffer, 0, MAX_BUFF_SIZE);
    while (true) {
        bytes_len = read(this->socket, buffer, MAX_BUFF_SIZE);
        if (bytes_len < 0) {
            if (errno == EAGAIN)break;
        } else if (bytes_len == 0) {
            break;
        } else {
            for (int i = 0; i < bytes_len; ++i)
                this->inputBuffer.push_back(buffer[i]);
            memset(buffer, 0, MAX_BUFF_SIZE);
        }
    }
}


#endif //ANET_TCPCONNECTION_H
