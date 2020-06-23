//
// Created by hhhxiao on 5/10/20.
//

#include "utils.h"
#include <chrono>
#include <fcntl.h>
#include <ctime>
#include <cstring>

std::string now() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    const char *str = std::ctime(&now);
    std::string s(str);
    s[s.length() - 1] = 0;
    return s;
}


void expect(bool val, const char *msg) {
    if (!val) {
        throw std::runtime_error(msg);
    }
}


void debug(const char *fmt, ...) {
#ifdef DEBUG
    printf("[%s DEBUG] ", now().c_str());
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
#endif

}

void info(const char *fmt, ...) {
    printf("[%s INFO] ", now().c_str());
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}

void set_no_blocking(int fd) {
    int flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}




