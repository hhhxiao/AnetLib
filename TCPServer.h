//
// Created by hhhxiao on 4/28/20.
//

#ifndef ANET_TCPSERVER_H
#define ANET_TCPSERVER_H

#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <netinet/in.h>
#include "Poller.h"
#include <cstring>
#include <utility>
#include <arpa/inet.h>
#include "utils.h"
#include "TcpConnection.h"
#include "ThreadPool.h"

class TCPServer {
private:
    ThreadPool threadPool;

    static void initAddress(struct sockaddr_in *address, int port);

    uint32_t port;
    int fd;
    Poller *poller;

    void accept();


    std::function<void(TcpConnection *)> connBuildEvent;
    std::function<void()> connClosedEvent;
public:
    explicit TCPServer(int port);

    void onConnBuild(std::function<void(TcpConnection *)> todo);

    void exitIf() {
    }

    void onConnClosed(std::function<void()> todo);

    void start();

    ~TCPServer() {
        delete poller;
        ::close(this->fd);
    }
};


void TCPServer::start() {
    expect(poller, "start failure: nullptr(poller)");
    log("server start");
    this->poller->loop_wait();
}

TCPServer::TCPServer(int port) : port(port) {
    this->poller = new Poller();
    struct sockaddr_in address{};
    initAddress(&address, port);
    this->fd = socket(PF_INET, SOCK_STREAM, 0);
    expect(fd != -1, "socket create error");
    set_no_blocking(this->fd);
    int result = bind(this->fd, (struct sockaddr *) &address, sizeof(address));
    expect(result != -1, "binding error");
    result = listen(this->fd, 5);
    expect(result != -1, "listener error");
    auto listener = new IOListener(this->fd, EPOLLIN);
    listener->setReadEvent([this]() { this->accept(); });
    this->poller->addListener(listener);
    info("server config finish");
}


void TCPServer::initAddress(struct sockaddr_in *address, int port) {
    if (address != nullptr) {
        memset(address, 0, sizeof(*address));
        address->sin_addr.s_addr = htonl(INADDR_ANY);
        address->sin_port = htons(port);
        address->sin_family = AF_INET;
    }
}

void TCPServer::accept() {
    struct sockaddr_in client_address{};
    socklen_t len;
    int client_socket = ::accept(this->fd, (
            struct sockaddr *) &client_address, &len);
    set_no_blocking(client_socket);

    //add to thread pool;
    threadPool.enqueue([this, client_socket]() {
        auto *conn = new TcpConnection(client_socket);
        this->connBuildEvent(conn);
        this->poller->addListener(conn->getListener());
    });
}


void TCPServer::onConnBuild(std::function<void(TcpConnection *)> todo) {
    this->connBuildEvent = std::move(todo);
}


void TCPServer::onConnClosed(std::function<void()> todo) {
    this->connClosedEvent = std::move(todo);
}

#endif //ANET_TCPSERVER_H
