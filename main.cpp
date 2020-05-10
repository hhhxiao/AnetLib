#include <functional>
#include "TCPServer.h"
#include <iostream>

using namespace std;

int main() {
    TCPServer server(8080);
    server.onConnBuild([=](TcpConnection *conn) {
        printf("a new connection\n");
        conn->onRead([&conn] {
            conn->sendMessage("hello world");
        });
    });
    server.start();
    return 0;
}
