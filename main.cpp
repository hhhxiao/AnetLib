#include <functional>
#include "TCPServer.h"
#include <iostream>
#include "DataBuffer.h"
#include <numeric>
#include <map>
#include <fstream>

using namespace std;

void f() {

}

int main() {

    //thread pool with 16 threads
    ThreadPool pool(16);
    TCPServer server(8888, &pool);
    int i = 0;
    std::map<TcpConnection *, FILE *> connList;
    FILE *fp = fopen("a.txt", "w+");

    server.onConnBuild([&connList, &i, fp](TcpConnection *conn) {
        //a new connection build
        ++i;
        printf("%d has connected\n,there has %lu connections\n", i, connList.size());
        conn->onByteRead([&connList, &conn, &fp](char *buffer, std::size_t num) {
            printf("%p read %lu bytes\n", conn, num);
            for (int i = 0; i < num; ++i) {
                fputc(buffer[i], fp);
            }
          //  fputs("\n----------------------------------\n", fp);
        });

        conn->onClose([conn, &connList] {
            if (!conn)return;
            printf("%p has closed\n", conn);
        });
    });
    server.start();
    return 0;
}






