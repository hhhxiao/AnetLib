//
// Created by hhhxiao on 4/28/20.
//

#ifndef ANET_TCPCONNECTION_H
#define ANET_TCPCONNECTION_H

#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <netinet/in.h>
#include "Poller.h"
#include <cstring>
#include <arpa/inet.h>

class TCPConnection {
private:
    uint32_t port;
    int fd;
    std::shared_ptr<Poller> poller;
public:
    TCPConnection(Poller *poller, int port);

    void start() const;

    void onRead();

    void onWrite();
};

void TCPConnection::start() const {
    printf("try accept\n");
    struct sockaddr_in clientAddress{};
    socklen_t len;
    int sock = accept(this->fd, (struct sockaddr *) &clientAddress, &len);
    const char *msg = "Hello world";
    write(sock, msg, strlen(msg) + 1);
    //todo
    //this->poller->addListener(sock);
}


void TCPConnection::onWrite() {

}

void TCPConnection::onRead() {

}

TCPConnection::TCPConnection(Poller *poller, int port) : port(port) {

    struct sockaddr_in address{};
    memset(&address, 0, sizeof(sockaddr));
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htonl(port);
    address.sin_family = AF_INET;

    this->fd = socket(PF_INET, SOCK_STREAM, 0);
    printf("create socket\n");
    assert(this->fd != -1);
    int result = bind(this->fd, (struct sockaddr *) &address, sizeof(address));
    printf("binding!\n");
    assert(result != -1);
    result = listen(this->fd, 5);
    printf("listening\n");
    assert(result != -1);
}


#endif //ANET_TCPCONNECTION_H
