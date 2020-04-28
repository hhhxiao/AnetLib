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
    std::function<void()> readEvent;
    std::function<void()> writeEvent;
public:
    explicit IOListener(int fd) : fd(fd) {}

    void setReadEvent(std::function<void()> task) {
        this->readEvent = std::move(task);
    }

    void setWriteEvent(std::function<void()> task) {
        this->writeEvent = std::move(task);
    }
    void onWrite(){
        this->writeEvent();
    }
    void onRead(){
        this->readEvent();
    }
};


#endif //ANET_IOLISTENER_H
