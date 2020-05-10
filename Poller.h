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
#include "utils.h"
#include <cstring>

#define  MAXSIZE 1024

class Poller {
private:
    int fd = -1;
    epoll_event events[MAXSIZE]{};
    std::vector<IOListener *> listeners;
public:
    Poller() {
        this->fd = epoll_create(MAXSIZE);
        expect(this->fd != -1, "poller create failure");
    }


    //none copyable
    Poller(const Poller &poller) = delete;

    Poller &operator=(const Poller &poller) = delete;

    void wait();

    [[noreturn]] void loop_wait();

    void addListener(IOListener *listener);

    void removeListener(IOListener *listener);
};


//wait once
void Poller::wait() {
    assert(this->fd != -1);
    int num = epoll_wait(this->fd, this->events, MAXSIZE, -1);
    for (int i = 0; i < num; ++i) {
        //todo:  error here need more about epoll
        auto listener = (IOListener *) this->events[i].data.ptr;
        unsigned event = events[i].events;
        if (listener) {
            printf("epoll from  %d-----%d\n", events[i].data.fd, listener->getFd());
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
    expect(listener, "[add listener:] receive a nullptr");
    struct epoll_event event{};
    memset(&event, 0, sizeof(event));
    event.events = listener->getEvent();
    event.data.fd = listener->getFd();
    event.data.ptr = listener;
    int r = epoll_ctl(this->fd, EPOLL_CTL_ADD, listener->getFd(), &event);
    expect(r != -1, "epoll ctl failure when add listener");
    this->listeners.push_back(listener);
}

void Poller::removeListener(IOListener *listener) {
}

#endif //ANET_POLLER_H
