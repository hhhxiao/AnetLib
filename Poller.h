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

#include <cstring>

#define  MAXSIZE 1024

class Poller {
private:
    int fd;


    void init();

    epoll_event events[MAXSIZE];
    std::vector<IOListener *> listeners;
public:
    Poller() : fd(-1) {
        init();
    }

    void wait();

    [[noreturn]] void loop_wait();

    void addListener(IOListener *listener);
};


//wait once
void Poller::wait() {
    assert(this->fd != -1);
    int num = epoll_wait(this->fd, this->events, MAXSIZE, -1);
    for (int i = 0; i < num; ++i) {
        IOListener *listener = (IOListener *) this->events[i].data.ptr;
        int event = events[i].events;
        if (listener) {
            if (event & EPOLLIN) {
                listener->onRead();
            }
            if (event & EPOLLOUT) {
                listener->onWrite();
            }
        }
    }
}


[[noreturn]] void Poller::loop_wait() {
    while (true)
        wait();
}

void Poller::addListener(IOListener *listener) {
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = listener->getEvent();
    event.data.ptr = listener;
    int r = epoll_ctl(this->fd, EPOLL_CTL_ADD, listener->getFd(), &event);
    if (r < 0) {
        std::cout << "epoll ctl failure\n";
    }
}

void Poller::init() {
    this->fd = epoll_create(MAXSIZE);
    if (fd == -1)
        throw std::runtime_error("epoll create failure");
}


#endif //ANET_POLLER_H
