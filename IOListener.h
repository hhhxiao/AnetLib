//
// Created by hhhxiao on 4/28/20.
//

#ifndef ANET_IOLISTENER_H
#define ANET_IOLISTENER_H

#include <functional>
#include <utility>

class IOListener {
private:
    int fd;
    const unsigned event;
    std::function<void()> readEvent;
    std::function<void()> writeEvent;
public:
    IOListener(int fd, unsigned events);
    void setReadEvent(std::function<void()> task) {
        this->readEvent = std::move(task);
    }
    void setWriteEvent(std::function<void()> task) {
        this->writeEvent = std::move(task);
    }
    void onWrite() {
        this->writeEvent();
    }

    void onRead() {
        this->readEvent();
    }

    int getFd() const {
        return this->fd;
    }

    unsigned getEvent() const {
        return this->event;
    }
};

IOListener::IOListener(int fd, unsigned int events) : fd(fd), event(events) {}


#endif //ANET_IOLISTENER_H
