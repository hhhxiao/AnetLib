#include <iostream>
#include <thread>
#include <functional>
#include "ThreadPool.h"
#include <condition_variable>
#include <mutex>
#include <queue>
#include <mutex>

void callTask(int a, int b) {
    std::cout << a + b;
}

int main() {

    return 0;
}
