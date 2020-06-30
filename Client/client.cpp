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


//first send file size(Byte) --8byte

#define BUFF_SIZE 512

uint64_t getFileLength(const char *filePath) {
    int len;
    uint64_t total_len = 0;
    char buffer[BUFF_SIZE];
    FILE *fp = fopen(filePath, "rb");
    if (!fp) {
        printf("error: %s\n", strerror(errno));
        return 0;
    }
    while (true) {
        len = fread((void *) buffer, 1, BUFF_SIZE, fp);
        total_len += len;
        if (len < BUFF_SIZE) {
            break;
        }
    }
    fclose(fp);
    return total_len;
}


//upload function
void buildConnect(sockaddr_in *address, int id, long *timeList, uint64_t fileSize) {

    int sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    expect(sock_fd != -1, "socket create failure\n");
    int result = connect(sock_fd, (struct sockaddr *) address, sizeof(*address));
    expect(result != -1, "connect failure\n");
    auto start = std::chrono::system_clock::now();
    char buffer[BUFF_SIZE];
    FILE *fp = fopen("rand.txt", "rb");

    //send file size
    write(sock_fd, &fileSize, sizeof(uint64_t));
    printf("send file len: %lu\n", fileSize);
    int len;
    while (true) {
        len = fread((void *) buffer, 1, BUFF_SIZE, fp);
        if (len < BUFF_SIZE) {
            write(sock_fd, buffer, len);
            break;
        }
        write(sock_fd, buffer, BUFF_SIZE);
    }
    fclose(fp);
    memset(buffer, 0, sizeof(buffer));
    read(sock_fd, buffer, BUFF_SIZE - 1);
    printf("receive %s\n", buffer);
    // printf(" thread  %d finished with message %s\n", id, buffer);
    auto s = std::chrono::duration_cast
            <std::chrono::microseconds>
            (std::chrono::system_clock::now() - start);
    timeList[id] = s.count();
}

int main(int argc, const char *argv[]) {

    if (argc != 4) {
        printf("usage [port]  [fileName] [thread num]");
        return 0;
    }

    //init address
    struct sockaddr_in server_address{};
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(argv[1]);
    server_address.sin_port = htons(atoi(argv[2]));

    int thread_num = atoi(argv[3]);
    std::vector<std::thread> threadList;
    long *timeList = new long[thread_num];
    const char *fileName = "rand.txt";
    uint64_t len = getFileLength(fileName);
    if (len <= 0) {
        printf("invalid file");
        return -1;
    }


    threadList.reserve(thread_num);
    for (int i = 0; i < thread_num; ++i)
        threadList.emplace_back(buildConnect, &server_address, i, timeList, len);
    for (auto &thread :threadList)
        thread.join();
    long totalTime = 0;
    for (int i = 0; i < thread_num; i++)
        totalTime += timeList[i];
    printf("\nuse %d threads, average send %lu bytes, cost time is %ld us\n",
           thread_num,
           len,
           totalTime / thread_num
    );

    delete[] timeList;
    return 0;
}


