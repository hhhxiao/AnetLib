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
    std::queue<char> inputBuffer;
    std::function<void()> readEvent;
    std::function<void()> closeEvent;
    std::function<void(char *, size_t)> readByteEvent;
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

    void onByteRead(std::function<void(char *buffer, size_t num)>);

    int readBytes(char *buff, int len) {
        int real_len = 0;
        while (!inputBuffer.empty() && real_len < len) {
            buff[real_len] = inputBuffer.front();
            inputBuffer.pop();
            ++real_len;
        }
        return real_len;
    }

    std::string getText() {
        std::string s;
        while (!inputBuffer.empty()) {
            s.push_back(inputBuffer.front());
            inputBuffer.pop();
        }
        return s;
    }

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
 * init the listener event
 * set up callback event
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
            this->readEvent();
        });
    });

    this->listener->setCloseEvent([this]() {
        this->closeEvent();
        this->close();
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
    this->close();
}

void TcpConnection::sendMessage(const char *msg) const {
    ::write(this->socket, msg, strlen(msg));
}


/**
 * read all bytes
 */
void TcpConnection::readAll() { //no linting
    //  printf("read All\n");
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
            this->readByteEvent(buffer, bytes_len);
            //todo: this is a bug  //how to save the buffer
            memset(buffer, 0, MAX_BUFF_SIZE);
        }
    }
}

void TcpConnection::onByteRead(std::function<void(char *buff, size_t num)> todo) {
    this->readByteEvent = std::move(todo);
}

#endif //ANET_TCPCONNECTION_H
