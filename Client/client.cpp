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


void buildConnect(sockaddr_in *address, int id) {
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    expect(sock != -1, "socket create failure\n");
    int result = connect(sock, (struct sockaddr *) address, sizeof(*address));
    expect(result != -1, "connect failure\n");
    printf("thread: %d build connect\n", id);
    char buffer[65];
    buffer[64] = 0;
    FILE *fp = fopen("rand.txt", "rb");
    int len = 0;
    while ((len = fread(buffer, 64, 1, fp)) > 0) {
        write(sock, buffer, len);
    }
    close(sock);
}

int main(int argc, const char *argv[]) {
    if (argc != 3) {
        printf("usage [port]  [fileName]");
        return 0;
    }

    struct sockaddr_in server_address{};
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(argv[1]);
    server_address.sin_port = htons(atoi(argv[2]));

    std::vector<std::thread> threadList;
    threadList.reserve(10);
    for (int i = 0; i < 10; ++i)
        threadList.emplace_back(buildConnect, &server_address, i);
    for (auto &thread :threadList)
        thread.join();
    return 0;
}
