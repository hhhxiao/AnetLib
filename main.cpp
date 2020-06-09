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
    std::map<TcpConnection *, int> connectionList;
    server.onConnBuild([&connectionList, &i](TcpConnection *conn) {
        connectionList.insert({conn, i});
        printf("%d has connected\n",i);
        ++i;
        conn->onRead([conn, &connectionList] {
            printf("from:%d  %s  \n", connectionList[conn], conn->getText().c_str());
        });
        conn->onClose([conn, &connectionList] {
            printf("%d has closed\n", connectionList[conn]);
        });
    });
    server.start();
    return 0;
}



