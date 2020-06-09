//
// Created by hhhxiao on 5/13/20.
//
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
static int keepRunning = 1;

void intHandler(int a) {
    keepRunning = 0;
}

int main(int argc, char *argv[]) {
    struct sigaction act;
    act.sa_handler = intHandler;
    sigaction(SIGINT, &act, NULL);
    while (keepRunning) {
    }
}