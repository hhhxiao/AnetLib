#include <functional>
#include "TCPServer.h"
#include <iostream>

using namespace std;

int main() {
    TCPServer server(8808);
    server.onConnBuild([=](TcpConnection *conn) {
        printf("a new connection\n");
    });
    server.start();

    return 0;
}

