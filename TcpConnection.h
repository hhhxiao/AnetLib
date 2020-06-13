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

#define MAX_BUFF_SIZE 512

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
    std::queue<char> inputBuffer;
    std::function<void(TcpConnection *)> readEvent;
    std::function<void(TcpConnection *)> closeEvent;
    std::function<void(TcpConnection *, char *, size_t)> readByteEvent;
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

    void onRead(std::function<void(TcpConnection *)> &&todo) { this->readEvent = std::move(todo); }

    void onClose(std::function<void(TcpConnection *)> &&todo) { this->closeEvent = std::move(todo); }

    void onRead(const std::function<void(TcpConnection *)> &todo) { this->readEvent = todo; }

    void onClose(const std::function<void(TcpConnection *)> &todo) { this->closeEvent = todo; }

    void onByteRead(const std::function<void(TcpConnection *, char *, size_t)> &todo) { this->readByteEvent = todo; }

    void onByteRead(std::function<void(TcpConnection *, char *buffer, size_t num)> &&todo) {
        this->readByteEvent = std::move(todo);
    };

    inline void close() {
        poller->removeListener(this->listener);
        ::close(this->socket);
        //   this->socket = -1;
    }

    inline IOListener *getListener() {
        return this->listener;
    }
};


/**
 * init a tcp connection
 * @param socket socket  the accept file descriptor
 * @param threadPool a thread pool pointer
 * @param poller  a poller pointer
 *
 * init the listener eventType
 * set up callback eventType
 * add listener to poller
 */
TcpConnection::TcpConnection(int socket, ThreadPool *threadPool, Poller *poller)
        : socket(socket), pool(threadPool),
          poller(poller) {
    expect(threadPool, "[TcpConnection: get a nullptr of thread pool]");
    expect(poller, "[TcpConnection: get a nullptr of poller]");

    this->listener = new IOListener(socket, EPOLLIN | EPOLLET | EPOLLRDHUP);
    this->listener->setReadEvent([this] {
        this->pool->enqueue([this]() {
            this->readAll();
        });
    });

    this->listener->setCloseEvent([this]() {
        this->closeEvent(this);
        this->close();
    });

    this->poller->addListener(listener);
}


TcpConnection::~TcpConnection() {
    this->close();
}

void TcpConnection::sendMessage(const char *msg) const {
    ::write(this->socket, msg, strlen(msg));
}


/**
 * read all bytes
 */
void TcpConnection::readAll() {
    //  printf("read All\n");
    int bytes_len;
    char buffer[MAX_BUFF_SIZE + 1];
    memset(buffer, 0, MAX_BUFF_SIZE + 1);
    while (true) {
        bytes_len = read(this->socket, buffer, MAX_BUFF_SIZE);
        if (bytes_len < 0) {
            if (errno == EAGAIN)break;
        } else if (bytes_len == 0) {
            break;
        } else {
            this->readByteEvent(this, buffer, bytes_len);
            //todo: this is a bug  //how to save the buffer
            memset(buffer, 0, MAX_BUFF_SIZE + 1);
        }
    }
}


#endif //ANET_TCPCONNECTION_H
