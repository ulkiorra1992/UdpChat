#include "tcpsocket.h"
#include "projectsettings.h"

TcpSocket::TcpSocket()
{

}

void TcpSocket::incomingConnection(int socketId)
{
    QTcpSocket *socket = new QTcpSocket(this);
    ProjectSettings::getInstance()->setPeerAdress(socket->peerAddress().toString());
}
