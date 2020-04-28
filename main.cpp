#include <iostream>
#include <thread>
#include <functional>
#include "ThreadPool.h"
#include <condition_variable>
#include <mutex>
#include <queue>
#include <mutex>
#include "TCPConnection.h"
void callTask(int a, int b) {
    std::cout << a + b;
}

#include <iostream>
using  namespace  std;
int main() {
    Poller poller;
    TCPConnection connection(&poller,8888);
    connection.start();
    return 0;
}
