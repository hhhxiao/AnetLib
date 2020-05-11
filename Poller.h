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
#include <iostream>

#define  MAXSIZE 1024

#include <set>

class Poller {
private:
    int fd = -1;
    epoll_event events[MAXSIZE]{};
    std::set<IOListener *> listeners;
public:
    Poller() {
        this->fd = epoll_create(MAXSIZE);
        expect(this->fd != -1, "poller create failure");
    }

    ~Poller() {
        if (this->fd > 0) {
            ::close(this->fd);
        }
    }

    //none copyable
    Poller(const Poller &poller) = delete;

    Poller &operator=(const Poller &poller) = delete;

    void wait();

    [[noreturn]] void loop_wait();

    void addListener(IOListener *listener);

    void removeListener(IOListener *listener);

    void changeListener(IOListener *listener, unsigned int event);
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
    this->listeners.insert(listener);
}

void Poller::removeListener(IOListener *listener) {
    expect(listener, "[remove listener:] receive a nullptr");
    int r = epoll_ctl(this->fd, EPOLL_CTL_DEL, listener->getFd(), nullptr);
    expect(r != -1, "epoll remove failure");
    this->listeners.erase(listener);
    delete listener;
}

void Poller::changeListener(IOListener *listener, unsigned int event) {
    expect(listener, "[remove listener:] receive a nullptr");
    auto r = this->listeners.find(listener);
    if (r == listeners.end())return;
    struct epoll_event epoll_event{};
    epoll_event.events = event;
    epoll_event.data.fd = listener->getFd();
    epoll_event.data.ptr = listener;
    epoll_ctl(this->fd, EPOLL_CTL_MOD, listener->getFd(), &epoll_event);
}


#endif //ANET_POLLER_H
