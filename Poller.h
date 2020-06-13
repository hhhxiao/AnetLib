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
#include <cerrno>

#define  MAX_EPOLL_SIZE 128

#include <set>

/**
 *Poller
 * a simple IO eventType detector
 */

class Poller {
private:
    bool epolling = true;
    int fd = -1;
    epoll_event events[MAX_EPOLL_SIZE]{};
    std::set<IOListener *> listeners;
public:
    Poller() {
        this->fd = epoll_create(MAX_EPOLL_SIZE);
        expect(this->fd != -1, "poller create failure");
    }

    ~Poller() {
        if (this->fd > 0) {
            ::close(this->fd);
        }
    }

    void setExit();

    //none copyable
    Poller(const Poller &poller) = delete;

    Poller &operator=(const Poller &poller) = delete;

    void wait();

    void loop_wait();

    void addListener(IOListener *listener);

    void removeListener(IOListener *listener);

    void changeListener(IOListener *listener, unsigned int event);
};


//wait once
void Poller::wait() {
    assert(this->fd != -1);
    int num = epoll_wait(this->fd, this->events, MAX_EPOLL_SIZE, -1);
    //  printf("epoll eventType\n");
    for (int i = 0; i < num; ++i) {
        auto listener = (IOListener *) this->events[i].data.ptr;
        unsigned event = events[i].events;
        if (listener) {
            if (event & EPOLLIN) {
                if (listener->getFd() == STDIN_FILENO) {
                    this->epolling = false;
                }
                debug("read event");
                listener->onRead();
            }
            if (event & EPOLLOUT) {
                debug("write event");
                listener->onWrite();
            }
            if (event % EPOLLRDHUP) {
                debug("close event");
                listener->onClose();
            }

            if (event & EPOLLERR) {
                info("epoll error event\n");
            }
        }
    }
}

void Poller::loop_wait() {
    while (epolling)
        wait();
    ::close(this->fd);
}


void Poller::addListener(IOListener *listener) {
    expect(listener, "[add listener:] receive a nullptr");
    struct epoll_event event{};
    memset(&event, 0, sizeof(event));
    event.events = listener->getEvent();
    event.data.fd = listener->getFd();
    event.data.ptr = listener;
    int r = epoll_ctl(this->fd, EPOLL_CTL_ADD, listener->getFd(), &event);
    expect(r != -1, strerror(errno));
    this->listeners.insert(listener);
}


void Poller::removeListener(IOListener *listener) {
    if (!listener)
        return;
    epoll_ctl(this->fd, EPOLL_CTL_DEL, listener->getFd(), nullptr);
    this->listeners.erase(listener);
    //delete listener;
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

void Poller::setExit() {
    auto listener = new IOListener(STDIN_FILENO, EPOLLIN);
    listener->setReadEvent([] {
        debug("server exited");
    });
    this->addListener(listener);
}


#endif //ANET_POLLER_H
