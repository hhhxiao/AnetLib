#include <functional>
#include "TCPServer.h"
#include <iostream>
#include "DataBuffer.h"
#include <numeric>

using namespace std;

int main() {
    ThreadPool pool;
    TCPServer server(8888, &pool);
    server.onConnBuild([=](TcpConnection *conn) {
        printf("a new connection\n");
        conn->onRead([conn] {
            conn->sendMessage(conn->getText().c_str());
        });
        conn->onClose([conn] {
            printf("a connection has closed");
        });
    });
    server.start();

    return 0;
}
