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
    setWindowIcon(QIcon(":/img/server.png"));

/* Инициализируем иконку трея, устанавливаем иконку своего приложения,
 * а также задаем всплывающую подсказку
 */
    trayIcon_ = new QSystemTrayIcon(this);
    trayIcon_->setIcon(QIcon(":/img/server.png"));
    trayIcon_->setToolTip("Сервер");
//
// ============= Создаем контекстное меню из двух пунктов =================== //
    QMenu *menu = new QMenu(this);
    QAction *viewWindow = new QAction("Открыть окно Сервера", this);
    QAction *quitWindow = new QAction("Выход", this);
//
    connect(viewWindow, SIGNAL(triggered(bool)), this, SLOT(show()));
    connect(quitWindow, SIGNAL(triggered(bool)), this, SLOT(close()));

    menu->addAction(viewWindow);
    menu->addAction(quitWindow);

// Установка контекстного меню на иконку и отображение иконки приложения в трее
    trayIcon_->setContextMenu(menu);
    trayIcon_->show();

// === Подключение сигнала нажатия на иконку к обработчику данного нажатия == //
    connect(trayIcon_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(onIconActivated(QSystemTrayIcon::ActivationReason)));

// Определение своего ip адреса
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
                             qint8 typeDatagram, QString state, QString msg,
                             QString name)
{
    QByteArray baDatagram;
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out << typeDatagram << state << clients_ << msg << name;
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
        QString name = UserSettings::getAuthorizationUser(login, password);
        QString ipAdress = UserSettings::getUserAdress(toSendUser);
        int port = UserSettings::getUserPort(toSendUser);

        transferMessage(ipAdress, port, type, state, message, name);
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
        QString name = UserSettings::getAuthorizationUser(login, password);
        QString ipAdress = UserSettings::getUserAdress(toSendUser);
        int port = UserSettings::getUserPort(toSendUser);

        transferMessage(ipAdress, port, type, state, message, name);
    }

// Фича отправка сигнала о том что собеседник набирает сообщение
    if (typeDatagram == 'T') {
        type = 'T';
        state = "write";
        QString name = UserSettings::getAuthorizationUser(login, password);
        QString ipAdress = UserSettings::getUserAdress(toSendUser);
        int port = UserSettings::getUserPort(toSendUser);
        ui->textEdit->append(ipAdress + "  " + port + "  " + QString("%0").arg(type));

        transferMessage(ipAdress, port, type, state, message, name);
    }
}

void Server::on_startServer_triggered()
{
    udpSocket_->bind(55555);
}

void Server::onIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason) {
    case QSystemTrayIcon::Trigger:
        // Событие игнорируется в том случае, если чекбокс не отмечен
        if (ui->appTray->isChecked()) {
            /* иначе, если окно видимо, то оно скрывается, и наоборот,
             * если скрыто, то разворачивается на экран
             */
            if(!this->isVisible()){
                this->show();
            } else {
                this->hide();
            }
        }
        break;
    default:
        break;
    }
}

void Server::closeEvent(QCloseEvent *event)
{
/* Если окно видимо и чекбокс отмечен, то завершение приложения игнорируется, а
 * окно просто скрывается, что сопровождается соответствующим всплывающим сообщением
 */
    if (this->isVisible() && ui->appTray->isChecked()) {
        event->ignore();
        this->hide();
        QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(
                    QSystemTrayIcon::Information);
        trayIcon_->showMessage("Сервер",
                               "Приложение свернуто в трей. Для того чтобы, "
                               "развернуть окно приложения, щелкните по иконке приложения в трее",
                               icon, 4000);
    }
}
