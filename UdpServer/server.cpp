#include "server.h"
#include "ui_server.h"
#include "projectsettings.h"
#include "usersettings.h"

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

void Server::responseRegistration(QHostAddress remoteAddr, quint16 remotePort,
                              qint8 typeDatagram, QString state)
{
    QByteArray baDatagram;
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out << typeDatagram << state;
    udpSocket_->writeDatagram(baDatagram, QHostAddress(remoteAddr), remotePort);
}

void Server::responseAuthorization(QString remoteAddr, int remotePort,
                                   qint8 typeDatagram, QString state)
{
    QByteArray baDatagram;
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out << typeDatagram << state << clients_;
    udpSocket_->writeDatagram(baDatagram, QHostAddress(remoteAddr), remotePort);
}

void Server::transferMessage(QString remoteAddr, quint16 remotePort,
                             qint8 typeDatagram, QString state, QString msg)
{
    QByteArray baDatagram;
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out << typeDatagram << state << clients_ << msg;
    udpSocket_->writeDatagram(baDatagram, QHostAddress(remoteAddr), remotePort);
}

void Server::onProcessDatagram()
{
    QByteArray baDatagram;
    qint8 typeDatagram;
    qint8 type;
    QString state;
    QString login;
    QString userName;
    QString password;
    QString toSendUser;
    QString message;
    QHostAddress remoteAddr;
    quint16 remotePort;
    QDateTime dateTime = QDateTime::currentDateTime();

    do {
        baDatagram.resize(udpSocket_->pendingDatagramSize());
        udpSocket_->readDatagram(baDatagram.data(), baDatagram.size(),
                                  &remoteAddr, &remotePort);
    } while(udpSocket_->hasPendingDatagrams());

    QDataStream in(&baDatagram, QIODevice::ReadOnly);
    in >> typeDatagram;
    in >> login;
    in >> userName;
    in >> password;
    in >> toSendUser;
    in >> message;

// Регистрация нового пользователя
    if (typeDatagram == 'R') {
        ProjectSettings::getInstance()->setUserRegistrationLogin(login);
        ProjectSettings::getInstance()->setUserRegistrationNickName(userName);
        ProjectSettings::getInstance()->setUserRegistrationPassword(password);

        bool stateReg = UserSettings::getRegistrationUser();
        type = 'R';
        if(stateReg) {
            state = "success";
        } else {
            state = "failure";
        }
        responseRegistration(remoteAddr, remotePort, type, state);
    }

// Авторизация на сервере зарегистрированного пользователя
    if (typeDatagram == 'A') {
        type = 'A';
        ProjectSettings::getInstance()->setUserAuthorizationLogin(login);
        ProjectSettings::getInstance()->setUserAuthorizationPassword(password);
        ProjectSettings::getInstance()->setUserAuthorizationIpAdress(remoteAddr.toString());
        ProjectSettings::getInstance()->setUserAuthorizationPort(remotePort);

        QString name = UserSettings::getAuthorizationUser(login, password);

        if (name.isEmpty()) {
            state = "failure";
            responseAuthorization(remoteAddr.toString(), remotePort, type, state);
        } else {
            state = "success";
            clients_.insert(login, name);
        }

        Q_FOREACH(QString userLogin, clients_.keys()) {
            QString ipAdress = UserSettings::getUserAdress(userLogin);
            int port = UserSettings::getUserPort(userLogin);
            responseAuthorization(ipAdress, port, type, state);
        }
    }

// Прием и пересылка сообщения
    if (typeDatagram =='M') {
        type = 'M';
        state = "success";
        ui->textEdit->append(QString("Время:%0 login=%1, message=%2").arg(dateTime.toString("[hh:mm:ss] "))
                             .arg(toSendUser).arg(message));
        QString ipAdress = UserSettings::getUserAdress(toSendUser);
        int port = UserSettings::getUserPort(toSendUser);

        transferMessage(ipAdress, port, type, state, message);
    }

// Удаление пользователя при его отключении от сервера
    if (typeDatagram == 'Q') {
        type = 'Q';
        state = "success";
        clients_.remove(login);

        Q_FOREACH(QString userLogin, clients_.keys()) {
            QString ipAdress = UserSettings::getUserAdress(userLogin);
            int port = UserSettings::getUserPort(userLogin);
            responseAuthorization(ipAdress, port, type, state);
            ui->textEdit->append(clients_.value(userLogin));
        }
    }

// Проверка подключен ли клиент к серверу
    if (typeDatagram == 'C') {
        type = 'C';
        state = "online";
        QByteArray baDatagram;
        QDataStream out(&baDatagram, QIODevice::WriteOnly);
        out << type << state << clients_;
        udpSocket_->writeDatagram(baDatagram, QHostAddress(remoteAddr), remotePort);
    }

// Подтверждение получения сообщения
    if (typeDatagram == 'V') {
        type = 'V';
        state = "success";
        QString ipAdress = UserSettings::getUserAdress(toSendUser);
        int port = UserSettings::getUserPort(toSendUser);

        transferMessage(ipAdress, port, type, state, message);
    }
}

void Server::on_startServer_triggered()
{
    udpSocket_->bind(55555);
}
