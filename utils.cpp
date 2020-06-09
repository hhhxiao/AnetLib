//
// Created by hhhxiao on 5/10/20.
//

#include "utils.h"
#include <chrono>
#include <fcntl.h>

void expect(bool val, const char *msg) {
    if (!val) {
        throw std::runtime_error(msg);
    }
}

void log(const char *msg) {
#ifdef DEBUG
    printf("[INFO] %s\n", msg);
#endif
}


void info(const char *info) {

    printf("[INFO] %s\n", info);
}

void set_no_blocking(int fd) {
    int flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}






