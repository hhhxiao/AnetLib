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
#include <fstream>
#include <set>

class TCPServer {
private:
    ThreadPool *threadPool;

    static void initAddress(struct sockaddr_in *address, int port);

    uint32_t port;
    int fd;
    Poller *poller;

    void accept();

    std::function<void(TcpConnection *)> connBuildEvent;
public:


    TCPServer(int port, ThreadPool *pool);


    void onConnBuild(std::function<void(TcpConnection *)> &&todo) { this->connBuildEvent = std::move(todo); }

    void onConnectionBuild(const std::function<void(TcpConnection *)> &todo) { this->connBuildEvent = todo; }

    void exitIf() {}

    void start();

    ~TCPServer() {
        delete poller;
        ::close(this->fd);
        // this->fd = -1;
    }
};


void TCPServer::start() {
    expect(poller, "start failure: nullptr(poller)");
    info("server start at: %d", this->port);
    this->poller->loop_wait();
}

TCPServer::TCPServer(int port, ThreadPool *pool) : port(port), threadPool(pool) {
    expect(pool, "get a nullptr of thread pool");
    this->poller = new Poller();
    poller->setExit();
    struct sockaddr_in address{};
    initAddress(&address, port);
    this->fd = socket(PF_INET, SOCK_STREAM, 0);
    expect(fd != -1, "socket create error");
    debug("create socket success");
    set_no_blocking(this->fd);
    int result = bind(this->fd, (struct sockaddr *) &address, sizeof(address));
    expect(result != -1, "binding error");
    debug("binging socket success");
    result = listen(this->fd, 5);
    expect(result != -1, "listener error");
    auto listener = new IOListener(this->fd, EPOLLIN);
    listener->setReadEvent([this] { this->accept(); });
    this->poller->addListener(listener);
    info("server init finished");
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
    socklen_t len = sizeof(client_address);
    int client_socket = ::accept(this->fd, (struct sockaddr *) &client_address, &len);
    if (client_socket == -1) {
        debug("fd is %d error is:%s\n", this->fd, strerror(errno));
    }
    expect(client_socket != -1, "connection error");

    char buff[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_address.sin_addr), buff, INET_ADDRSTRLEN);
    info("a new Tcp connection: %s", buff);

    set_no_blocking(client_socket);
    auto *conn = new TcpConnection(client_socket, this->threadPool, this->poller);
    this->connBuildEvent(conn);
}


#endif //ANET_TCPSERVER_H
