#include <QApplication>
#include "TcpServer.h"
#include "TcpClient.h"
#include "Point.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    int defaultPort = 2323;

    TcpServer server(defaultPort);
    server.show();

    TcpClient client("localhost", defaultPort);
    client.move(server.x() + server.width() + 20, server.y());
    client.show();

    return a.exec();
}
