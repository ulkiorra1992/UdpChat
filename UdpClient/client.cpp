#include "client.h"
#include "ui_client.h"

#include <QDateTime>

Client::Client(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Client)
{
    ui->setupUi(this);
    udpSocket_ = new QUdpSocket(this);
    connect(udpSocket_, SIGNAL(readyRead()), this, SLOT(onProcessDatagram()));
}

Client::~Client()
{
    delete ui;
}

void Client::on_Registration_triggered()
{

}

void Client::on_Authorization_triggered()
{

}

void Client::onProcessDatagram()
{

}

void Client::on_tbSend_clicked()
{
    QByteArray baDatagram;
    QDateTime dateTime = QDateTime::currentDateTime();
    qint8 state = 'M';
    int port = 55555;
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << state << ui->TeOut->toPlainText() << dateTime;
    udpSocket_->writeDatagram(baDatagram, QHostAddress("192.168.0.84"), port);
}
