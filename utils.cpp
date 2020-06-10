//
// Created by hhhxiao on 5/10/20.
//

#include "utils.h"
#include <chrono>
#include <fcntl.h>

const char *now() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    return std::ctime(&now);
}


void expect(bool val, const char *msg) {
    if (!val) {
        throw std::runtime_error(msg);
    }
}

void log(const char *msg) {
#ifdef DEBUG
    printf("[%s LOG]%s\n", now(), msg);
#endif
}


void info(const char *info) {
    printf("[%s INFO]%s\n", now(), info);
}

void set_no_blocking(int fd) {
    int flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}




