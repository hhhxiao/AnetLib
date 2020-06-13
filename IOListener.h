//
// Created by hhhxiao on 4/28/20.
//

#ifndef ANET_IOLISTENER_H
#define ANET_IOLISTENER_H

#include "utils.h"
#include <functional>
#include <utility>

class IOListener {
private:
    int fd;
    const unsigned eventType;
    std::function<void()> readEvent;
    std::function<void()> writeEvent;
    std::function<void()> closeEvent;
public:
    IOListener(const IOListener &) = delete;

    IOListener &operator=(const IOListener &) = delete;

    IOListener(int fd, unsigned events);

    void setReadEvent(std::function<void()> &&task) { this->readEvent = std::move(task); }

    void setReadEvent(const std::function<void()> &task) { this->readEvent = task; }

    void setWriteEvent(std::function<void()> &&task) { this->writeEvent = std::move(task); }

    void setWriteEvent(const std::function<void()> &task) { this->writeEvent = task; }

    void setCloseEvent(std::function<void()> &&task) { this->closeEvent = std::move(task); }

    void setClose(const std::function<void()> &task) { this->closeEvent = task; }


    void onWrite() {
        this->writeEvent();
    }


    void onRead() const {
        // printf("read eventType from %d\n", this->fd);
        //printf("Listener: onRead\n");
        this->readEvent();
    }


    inline int getFd() const {
        return this->fd;
    }

    inline unsigned getEvent() const {
        return this->eventType;
    }

    void onClose() {
        if (this->closeEvent)
            this->closeEvent();
    }
};

IOListener::IOListener(int fd, unsigned int events) : fd(fd), eventType(events) {
    expect(fd > 0, "[IO Listener] fd error");
}


#endif //ANET_IOLISTENER_H
