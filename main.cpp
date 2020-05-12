#include <functional>
#include "TCPServer.h"
#include <iostream>

using namespace std;

int main() {
    TCPServer server(8888);
    server.onConnBuild([=](TcpConnection *conn) {
        printf("a new connection\n");
        conn->onRead([conn] {
            conn->sendMessage(conn->getText().c_str());
        });
    });
    server.start();
    return 0;
}



