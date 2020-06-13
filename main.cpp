#include <functional>
#include "TCPServer.h"
#include <iostream>
#include "DataBuffer.h"
#include <numeric>
#include <map>
#include <fstream>
#include <memory>
#include <utility>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

int main() {
    ThreadPool pool;
    TCPServer server(8888, &pool);
    int i = 0;
    std::map<TcpConnection *, std::ofstream *> connList;
    server.onConnBuild([&](TcpConnection *connection) {
        ++i;
        auto fileName = std::to_string(i).append(".txt");
        connList[connection] = new std::ofstream(fileName);
        connection->onByteRead([&](TcpConnection *conn, char *buffer, std::size_t num) {
            auto stream = connList[conn];
            for (int i = 0; i < num; i++)
                (*stream) << buffer[i];
            stream->flush();
        });
        connection->onClose([&](TcpConnection *conn) {
            auto stream = connList[conn];
            stream->close();
            conn->close();
            connList.erase(conn);
            info("connection closed");
        });
    });
    server.start();
    return 0;
}






