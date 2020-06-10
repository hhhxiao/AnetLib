//
// Created by hhhxiao on 5/11/20.
//

#ifndef ANET_TCPCONNECTION_H
#define ANET_TCPCONNECTION_H

#include <utility>
#include <sys/epoll.h>
#include <queue>
#include "IOListener.h"
#include "Poller.h"

#define MAX_BUFF_SIZE 1024

/**
 * @author xiaohengying 2220/5/1
 * An abstract of Tcp connection
 */
#include "ThreadPool.h"
#include "DataBuffer.h"

class TcpConnection {
private:
    int socket;
    IOListener *listener;
    std::string inputBuffer;
    std::function<void()> readEvent;
    std::function<void()> closeEvent;
    Poller *poller{};

    void readAll();

    ThreadPool *pool;

public:
    void sendMessage(const char *msg) const;

    ~TcpConnection();

    //disable copy
    TcpConnection(const TcpConnection &) = delete;

    TcpConnection &operator=(const TcpConnection &) = delete;


    TcpConnection(int socket, ThreadPool *pool, Poller *poller);

    void onRead(std::function<void()> todo);

    void onClose(std::function<void()> todo);

    std::string getText() {
        std::string s(this->inputBuffer);
        this->inputBuffer.clear();
        return s;
    }

    inline void close() const {
        poller->removeListener(this->listener);
        //delete this->listener;
        ::close(this->socket);
    }

    inline IOListener *getListener() {
        return this->listener;
    }
};


TcpConnection::TcpConnection(int socket, ThreadPool *threadPool, Poller *poller)
        : socket(socket), pool(threadPool),
          poller(poller) {
    expect(threadPool, "[TcpConnection: get a nullptr of thread pool]");
    expect(poller, "[TcpConnection: get a nullptr of poller]");
    this->listener = new IOListener(socket, EPOLLIN | EPOLLET | EPOLLRDHUP);
    this->listener->setReadEvent([this] {
//        readAll();
        this->pool->enqueue([this]() {
            this->readAll();
            this->readEvent();
        });
    });
    this->listener->setCloseEvent([this]() {
        // this->closeEvent();
        //  this->close();
    });
    this->poller->addListener(listener);
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


//readAll byte in the buffer
void TcpConnection::readAll() {
    int bytes_len;
    char buffer[MAX_BUFF_SIZE + 1];
    memset(buffer, 0, MAX_BUFF_SIZE + 1);
  //  printf("read all\n");
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
