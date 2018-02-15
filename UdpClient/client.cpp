#include "client.h"
#include "ui_client.h"
#include "registrationusers.h"
#include "authorizationusers.h"

#include <QDateTime>

Client::Client(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Client)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/img/chat.png"));

/* Инициализируем иконку трея, устанавливаем иконку своего приложения,
 * а также задаем всплывающую подсказку
 */
    trayIcon_ = new QSystemTrayIcon(this);
    trayIcon_->setIcon(QIcon(":/img/chat.png"));
    trayIcon_->setToolTip("Клиент");
//
// ============= Создаем контекстное меню из двух пунктов =================== //
    QMenu *menu = new QMenu(this);
    QAction *viewWindow = new QAction("Открыть окно Чата", this);
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
    connect(trayIcon_, SIGNAL(messageClicked()), this, SLOT(onMessageClicked()));


    udpSocket_ = new QUdpSocket(this);
    udpSocket_->bind(55555);
    connect(udpSocket_, SIGNAL(readyRead()), this, SLOT(onProcessDatagram()));
}

Client::~Client()
{
    delete ui;
}

void Client::on_Registration_triggered()
{
    RegistrationUsers *reg = new RegistrationUsers;
    if(reg->exec() == QDialog::Accepted) {
        login_ = reg->getUserLogin();
        nickName_ = reg->getUserNickName();
        password_ = reg->getUserPassword();
        serverIp_ = reg->getServerIp();
        type_ = 'R';
        usersData();
    }
    delete reg;
}

void Client::on_Authorization_triggered()
{
    AuthorizationUsers * authorization = new AuthorizationUsers;
    if(authorization->exec() == QDialog::Accepted) {
        login_ = authorization->getUserLogin();
        password_ = authorization->getUserPassword();
        serverIp_ = authorization->getServerIp();
        type_ = 'A';
        usersData();
    }
    delete authorization;
}

void Client::onIconActivated(QSystemTrayIcon::ActivationReason reason)
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

void Client::usersData()
{
    QByteArray baDatagram;
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out << type_;
    out << login_;
    out << nickName_;
    out << password_;
    udpSocket_->writeDatagram(baDatagram, QHostAddress(serverIp_), 55555);
}

void Client::onProcessDatagram()
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

    in >> message;
    ui->TeMessageIn->append(message);

    // Пульсация значка приложения
    QApplication::alert(this);
    onShowMessage(message);
}

void Client::on_tbSend_clicked()
{
    QByteArray baDatagram;
    QDateTime dateTime = QDateTime::currentDateTime();
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << type_;
    udpSocket_->writeDatagram(baDatagram, QHostAddress(serverIp_), 55555);
}

void Client::onShowMessage(QString message)
{
    trayIcon_->showMessage("Новое сообщение", message,
                           QSystemTrayIcon::Information,3000);
}

void Client::onMessageClicked()
{
    this->show();
}

void Client::closeEvent(QCloseEvent *event)
{
    /* Если окно видимо и чекбокс отмечен, то завершение приложения игнорируется, а
     * окно просто скрывается, что сопровождается соответствующим всплывающим сообщением
     */
        if (this->isVisible() && ui->appTray->isChecked()) {
            event->ignore();
            this->hide();
            QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(
                        QSystemTrayIcon::Information);
            trayIcon_->showMessage("Клиент",
                                   "Приложение свернуто в трей. Для того чтобы, "
                                   "развернуть окно приложения, щелкните по иконке приложения в трее",
                                   icon, 4000);
        } else {

        }
}
