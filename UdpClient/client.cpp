#include "client.h"
#include "ui_client.h"
#include "registrationusers.h"
#include "authorizationusers.h"

#include <QDateTime>
#include <QFontDialog>
#include <QColorDialog>
#include <QCoreApplication>

Client::Client(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Client)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/img/chat.png"));
    setWindowTitle("Чат версии 0.1");

    pTimer_ = new QTimer(this);
    pTimer_->setInterval(3000);
    pTimer_->start();
    connect(pTimer_, SIGNAL(timeout()), this, SLOT(onConnectToServer()));

    state_ = new InquiryState();
    connect(state_, SIGNAL(responseReceived(bool)), this, SLOT(onServerState(bool)));

    ui->lblStateInfo->hide();

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

    connect(ui->listWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(onListWidgetUser(QModelIndex)));

    connect(ui->TeMessageOut, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
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
        ui->lblState->setText("Регистрация...");
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
        ui->lblState->setText("Авторизация...");
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

void Client::usersListWidget(QString user)
{
    QListWidgetItem *item = 0;
        item = new QListWidgetItem(user, ui->listWidget);
        item->setIcon(QPixmap(":/img/user.png"));
}

void Client::onConnectToServer()
{
    QByteArray baDatagram;
    type_ = 'C';
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out << type_;
    udpSocket_->writeDatagram(baDatagram, QHostAddress(serverIp_), 55555);
    state_->startTimerRequest();
}

void Client::onServerState(bool state)
{
    if (!state) {
        ui->lblState->setText("<b><font color=red>нет связи с сервером</font></b>");
    }
}

void Client::onTextChanged()
{
    QByteArray baDatagram;
    type_= 'T';
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out << type_;
    out << login_;
    out << users_.value(login_);
    out << password_;
    out << userLogin_;
    udpSocket_->writeDatagram(baDatagram, QHostAddress(serverIp_), 55555);
}


void Client::onProcessDatagram()
{
    QByteArray baDatagram;
    qint8 typeDatagram;
    QString state;
    QHostAddress remoteAddr;
    quint16 remotePort;
    QString receivedMessage;
    QString name;
    QDateTime dateTime = QDateTime::currentDateTime();

    do {
        baDatagram.resize(udpSocket_->pendingDatagramSize());
        udpSocket_->readDatagram(baDatagram.data(), baDatagram.size(),
                                  &remoteAddr, &remotePort);
    } while(udpSocket_->hasPendingDatagrams());

    QDataStream in(&baDatagram, QIODevice::ReadOnly);

    in >> typeDatagram >> state >> users_ >> receivedMessage >> name;

    if (typeDatagram == 'R') {
        state_->stopTimerRequest();
        if (state == "failure") {
            ui->lblStateInfo->show();
            ui->lblStateInfo->setText("Пользователь с таким логином существует!");
        } else {
            ui->lblStateInfo->hide();
            ui->lblStateInfo->setText("");
            ui->lblState->setText("Успех!");
        }
    }

    if (typeDatagram == 'A') {
        state_->stopTimerRequest();
        if (state == "failure") {
            ui->lblStateInfo->show();
            ui->lblStateInfo->setText("Не правильные логин или пароль!");
        } else {
            ui->lblStateInfo->hide();
            ui->lblStateInfo->setText("");
            ui->lblState->setText("Успех!");
            ui->listWidget->clear();
            Q_FOREACH(QString login, users_.keys()) { // получаем сисок логинов и по ним получаем список имен пользователей для отображения
                if (login != login_) {
                    usersListWidget(users_.value(login));
                }
                setWindowTitle("Пользователь: " + users_.value(login_));
            }
        }
    }

    if (typeDatagram == 'M') {
        state_->stopTimerRequest();
        ui->TeMessageIn->setTextColor(QColor(Qt::black));
        ui->TeMessageIn->append(name + dateTime.toString(" [hh:mm:ss]"));
        ui->TeMessageIn->setAlignment(Qt::AlignLeft);
        ui->TeMessageIn->setTextColor(colorInMsg_);
        ui->TeMessageIn->append(receivedMessage);

        onShowMessage(receivedMessage);

        QByteArray baDatagram;
        type_ = 'V';
        QDataStream out(&baDatagram, QIODevice::WriteOnly);
        out << type_;
        out << login_;
        out << nickName_;
        out << password_;
        out << userLogin_;
        udpSocket_->writeDatagram(baDatagram, QHostAddress(serverIp_), 55555);
    }

    if (typeDatagram == 'Q') {
        state_->stopTimerRequest();
        ui->listWidget->clear();
        userLogin_.clear();
        Q_FOREACH(QString login, users_.keys()) { // получаем сисок логинов и по ним получаем список имен пользователей для отображения
            if (login != login_) {
                usersListWidget(users_.value(login));
                setWindowTitle("Пользователь: " + users_.value(login_));
            }
        }
    }

    if (typeDatagram == 'C') {
        if (state == "online") {
            if (!users_.value(userLogin_).isEmpty()) {
                ui->lblStateInfo->show();
                ui->lblStateInfo->setText(QString("%0 %1")
                                          .arg(users_.value(userLogin_))
                                          .arg("(<b><font color=green>онлайн</font></b>)"));
            } else {
                ui->lblStateInfo->hide();
            }
            state_->stopTimerRequest();
            ui->lblState->setText("<b><font color=green>есть связь с сервером</font></b>");
        }
    }

    if (typeDatagram == 'T') {
        state_->stopTimerRequest();
        ui->lblStateInfo->show();
        ui->lblStateInfo->setText(name + " (<b><font color=green>печатает...</font></b>)");
    }
}

void Client::on_tbSend_clicked()
{
    QDateTime dateTime = QDateTime::currentDateTime();
    QByteArray baDatagram;
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out << type_;
    out << login_;
    out << nickName_;
    out << password_;
    out << userLogin_;
    out << ui->TeMessageOut->toPlainText();
    udpSocket_->writeDatagram(baDatagram, QHostAddress(serverIp_), 55555);

    ui->TeMessageIn->setAlignment(Qt::AlignRight);
    ui->TeMessageIn->setTextColor(QColor(Qt::black));
    ui->TeMessageIn->append(users_.value(login_) + dateTime.toString(" [hh:mm:ss]"));
    ui->TeMessageIn->setTextColor(colorOutMsg_);
    ui->TeMessageIn->append(ui->TeMessageOut->toPlainText());
//    ui->TeMessageIn->append(QString("%0%1/>%2").arg("<img src=:/img/").arg("3.gif").
//                            arg(ui->TeMessageOut->toPlainText()));
    ui->TeMessageOut->clear();
}

void Client::onShowMessage(QString message)
{
    // Пульсация значка приложения
    QApplication::alert(this);
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
            type_ = 'Q';
            usersData();
        }
}

void Client::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        type_ = 'M';
        on_tbSend_clicked();
    }
}


void Client::on_Quit_triggered()
{
    type_ = 'Q';
    usersData();
    qApp->exit();
}

void Client::onListWidgetUser(QModelIndex index)
{
    userLogin_ = users_.key(ui->listWidget->item(index.row())->text());
}

void Client::on_Mountains_triggered()
{
    QFile styleSheet(":/style/transparent.qss");

    if (!styleSheet.open(QIODevice::ReadOnly)) {
        qWarning("Unable to open :/style/transparent.qss");
        return;
    }

    qApp->setStyleSheet(styleSheet.readAll());
}


void Client::on_NoStyle_triggered()
{
    QFile styleSheet(":/style/nostyle.qss");

    if (!styleSheet.open(QIODevice::ReadOnly)) {
        qWarning("Unable to open :/style/nostyle.qss");
        return;
    }

    qApp->setStyleSheet(styleSheet.readAll());
}

void Client::on_font_triggered()
{
    bool ok;
     QFont font = QFontDialog::getFont(&ok, this);
     if (ok) {
        ui->TeMessageIn->setFont(font);
     }
}

void Client::on_ColorFontOut_triggered()
{
    QColor color = QColorDialog::getColor();
    if (color.isValid() ) {
        colorOutMsg_ = color;
    }
}

void Client::on_ColorFontIn_triggered()
{
    QColor color = QColorDialog::getColor();
    if (color.isValid() ) {
        colorInMsg_ = color;
    }
}
