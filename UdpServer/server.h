#ifndef SERVER_H
#define SERVER_H

#include "projectnamehelper.h"

#include <QMainWindow>
#include <QUdpSocket>

namespace Ui {
class Server;
}

class Server : public QMainWindow
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = 0);
    ~Server();

private:
    Ui::Server *ui;
    QUdpSocket *udpSocket_;
    QMap<QString, QString> clients_;
    QList<QString> users_;

    /**
     * @brief responseRegistration Регистрация нового пользователя
     * @param remoteAddr ip адрес пользователя
     * @param remotePort порт пользователя
     * @param typeDatagram тип датаграммы ('R')
     * @param state статус регистрации (успех/ошибка)
     */
    void responseRegistration(QHostAddress remoteAddr, quint16 remotePort,
                              qint8 typeDatagram, QString state);
    /**
     * @brief responseAuthorization авторизация пользователя
     * @param remoteAddr ip адрес пользователя
     * @param remotePort порт пользователя
     * @param typeDatagram тип датаграммы ('А')
     * @param state статус авторизации (успех/не верные логин или пароль)
     */
    void responseAuthorization(QString remoteAddr, int remotePort,
                               qint8 typeDatagram, QString state);
    /**
     * @brief transferMessage Пересылка сообщений между пользователями
     * @param remoteAddr ip адрес пользователя кому перенаправить сообщение
     * @param remotePort порт пользователя кому перенаправить сообщение
     * @param typeDatagram тип датаграммы ('М')
     * @param state статус
     * @param msg сообщение
     */
    void transferMessage(QString remoteAddr, quint16 remotePort,
                         qint8 typeDatagram, QString state, QString msg);

private slots:
    void onProcessDatagram();
    void on_startServer_triggered();

protected:

};

#endif // SERVER_H
