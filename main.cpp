#include <functional>
#include "TCPServer.h"
#include <iostream>
#include "DataBuffer.h"
#include <numeric>
#include <map>

using namespace std;

void f() {

}

int main() {
    ThreadPool pool;
    TCPServer server(8888, &pool);
    int i = 0;
    std::map<TcpConnection *, int> connList;
    std::map<int, FILE *> fileList;

    server.onConnBuild([&connList, &i, &fileList](TcpConnection *conn) {
        //a new connection build
        printf("%d has connected\n", i);
        connList.insert({conn, i});
        FILE *fp = fopen(std::to_string(i).append(".txt").c_str(), "wb");
        fileList.insert({i, fp});
        ++i;

        conn->onRead([conn, &connList, &fileList] {
            auto fp = fileList[connList[conn]];
            if (fp) {
                fputs(conn->getText().c_str(), fp);
            }
        });

        conn->onClose([conn, &connList, &fileList] {
            auto connIndex = connList[conn];
            printf("%d has closed\n", connIndex);
            auto fp = fileList[connIndex];
            if (fp) {
                fclose(fp);
            }
            fileList.erase(connIndex);
            connList.erase(conn);
        });
    });
    server.start();
    return 0;
}






