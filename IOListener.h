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
    IOListener(const IOListener &) = delete;

    IOListener &operator=(const IOListener &) = delete;

    IOListener(int fd, unsigned events);

    void setReadEvent(std::function<void()> task);

    void setWriteEvent(std::function<void()> task);


    void onWrite() {
        this->writeEvent();
    }


    void onRead() const {
        //printf("read event from %d\n", this->fd);
        this->readEvent();
    }


    inline int getFd() const {
        return this->fd;
    }

    inline unsigned getEvent() const {
        return this->event;
    }
};

IOListener::IOListener(int fd, unsigned int events) : fd(fd), event(events) {}

void IOListener::setWriteEvent(std::function<void()> task) {
    this->writeEvent = std::move(task);
}

void IOListener::setReadEvent(std::function<void()> task) {
    this->readEvent = std::move(task);
}


#endif //ANET_IOLISTENER_H
