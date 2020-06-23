//
// Created by hhhxiao on 5/28/20.
//


#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include "../Utils.h"
#include <cstring>
#include <thread>
#include <vector>
#include <mutex>

#define BUFF_SIZE 256

std::mutex mutex;

//upload function
void buildConnect(sockaddr_in *address, int id, long *timeList, size_t *bytes) {
    int sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    expect(sock_fd != -1, "socket create failure\n");
    int result = connect(sock_fd, (struct sockaddr *) address, sizeof(*address));
    expect(result != -1, "connect failure\n");
    auto start = std::chrono::system_clock::now();
    // printf("thread: %d build connect\n", id);
    char buffer[BUFF_SIZE];
    FILE *fp = fopen("rand.txt", "rb");
    int len;
    while (true) {
        len = fread((void *) buffer, 1, BUFF_SIZE, fp);
        printf("thread %d: send [%d] bytes\n", id, len);
        if (len < BUFF_SIZE) {
            write(sock_fd, buffer, len);
            bytes[id] += len;
            break;
        }
        write(sock_fd, buffer, BUFF_SIZE);
    }
    printf(" thread  %d finished\n", id);
    auto s = std::chrono::duration_cast
            <std::chrono::microseconds>
            (std::chrono::system_clock::now() - start);
    timeList[id] = s.count();
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
    long *timeList = new long[thread_num];
    auto *bytes = new size_t[thread_num];

    threadList.reserve(thread_num);
    for (int i = 0; i < thread_num; ++i)
        threadList.emplace_back(buildConnect, &server_address, i, timeList, bytes);
    for (auto &thread :threadList)
        thread.join();
    long totalTime = 0;
    size_t totalBytes = 0;
    for (int i = 0; i < thread_num; i++) {
        totalTime += timeList[i];
        totalBytes += bytes[i];
    }
    printf("\nuse %d threads, average send %lu bytes time is %ld us\n",
           thread_num,
           totalBytes / thread_num,
           totalTime / thread_num
    );

    delete[] timeList;
    return 0;
}


