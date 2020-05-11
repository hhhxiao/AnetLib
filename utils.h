//
// Created by hhhxiao on 5/10/20.
//

#ifndef ANET_UTILS_H
#define ANET_UTILS_H

#include <exception>

#define DEBUG

#include <cstdio>
#include <stdexcept>

void expect(bool val, const char *msg);

void log(const char *msg);

void info(const char *info);

void set_no_blocking(int fd);
#endif //ANET_UTILS_H
