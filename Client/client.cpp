//
// Created by hhhxiao on 5/28/20.
//


#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include "../utils.h"
#include <cstring>
#include <thread>
#include <vector>

#define BUFF_SIZE 64

//upload function
void buildConnect(sockaddr_in *address, int id) {
    int sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    expect(sock_fd != -1, "socket create failure\n");
    int result = connect(sock_fd, (struct sockaddr *) address, sizeof(*address));
    expect(result != -1, "connect failure\n");
    printf("thread: %d build connect\n", id);
    char buffer[BUFF_SIZE];
    FILE *fp = fopen("rand.txt", "rb");
    int len = 0;
    while (true) {
        len = fread((void *) buffer, 1, BUFF_SIZE, fp);
        if (len < BUFF_SIZE) {
            write(sock_fd, buffer, len);
            break;
        }
        write(sock_fd, buffer, BUFF_SIZE);
    }
    close(sock_fd);
}

int main(int argc, const char *argv[]) {
    if (argc != 4) {
        printf("usage [port]  [fileName] [thread num]");
        return 0;
    }

    struct sockaddr_in server_address{};
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(argv[1]);
    server_address.sin_port = htons(atoi(argv[2]));
    int thread_num = atoi(argv[3]);
    std::vector<std::thread> threadList;
    threadList.reserve(thread_num);
    for (int i = 0; i < thread_num; ++i)
        threadList.emplace_back(buildConnect, &server_address, i);
    for (auto &thread :threadList)
        thread.join();
    return 0;
}


