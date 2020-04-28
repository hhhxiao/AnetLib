//
// Created by hhhxiao on 4/28/20.
//

#ifndef ANET_POLLER_H
#define ANET_POLLER_H

#include <sys/epoll.h>
#include <vector>
#include <memory>
#include <sys/epoll.h>
#include <cassert>
#include "IOListener.h"

#define  MAXSIZE 1024

class Poller {
private:
    int fd;

    void wait();

    void loop_wait();

    epoll_event events[MAXSIZE];
    std::vector<std::shared_ptr<IOListener>> listeners;
public:
    Poller() : fd(-1) {
        init();
    }


    void init() {
        this->fd = epoll_create(MAXSIZE);
        if (fd == -1)
            throw std::runtime_error("epoll create failure");
    }
    void addListener(int) {

    }
};


void Poller::wait() {
    assert(this->fd != -1);
    int num = epoll_wait(this->fd, this->events, MAXSIZE, -1);
    for (int i = 0; i < num; ++i) {
        int f = events[i].data.fd;
        this->listeners[i].get()->onRead();
    }
}

void Poller::loop_wait() {
    while (true)
        wait();
}


#endif //ANET_POLLER_H
