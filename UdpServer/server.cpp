#include "server.h"
#include "ui_server.h"
#include "projectsettings.h"

#include <QNetworkInterface>
#include <QDateTime>
#include <QDebug>

Server::Server(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);

    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // используем первый не локальный адрес IPv4
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
                ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // если мы ни одного адреса не нашли, то используем IPv4 localhost
    if (ipAddress.isEmpty()) {
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    }
    setWindowTitle(ProjectNameHelper::appRu() + QString("  IP: %1").arg(ipAddress));


    udpSocket_ = new QUdpSocket(this);
    connect(udpSocket_, SIGNAL(readyRead()), this, SLOT(onProcessDatagram()));
}

Server::~Server()
{
    delete ui;
}

void Server::onProcessDatagram()
{
    QByteArray baDatagram;
    qint8 state;
    QHostAddress remoteAddr;
    quint16 remotePort;
    QDateTime dateTime;

    do {
        baDatagram.resize(udpSocket_->pendingDatagramSize());
        udpSocket_->readDatagram(baDatagram.data(), baDatagram.size(),
                                  &remoteAddr, &remotePort);
    } while(udpSocket_->hasPendingDatagrams());

    QDataStream in(&baDatagram, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);
    QString message;

    in >> state >>message >> dateTime;

    if (state =='M') {
        ui->textEdit->append(message);
    }

    qDebug() << state << remoteAddr << remotePort << message << dateTime;
}

void Server::on_startServer_triggered()
{
    udpSocket_->bind(55555);
}
