#include <iostream>
#include <thread>
#include <functional>
#include "ThreadPool.h"
#include <condition_variable>
#include <mutex>
#include <queue>
#include <mutex>
#include "TCPConnection.h"
#include <iostream>

using namespace std;

int main() {
    Poller poller;
    //set et mode
    IOListener ioListener(STDIN_FILENO, EPOLLIN | EPOLLET);
    ioListener.setReadEvent([]() {
        printf("data received \n");
    });
    poller.addListener(&ioListener);
    poller.loop_wait();
    return 0;
}