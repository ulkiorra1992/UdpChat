#ifndef SERVER_H
#define SERVER_H

#include "projectnamehelper.h"

#include <QMainWindow>
#include <QUdpSocket>
#include <QAction>
#include <QCloseEvent>
#include <QSystemTrayIcon>

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
    QSystemTrayIcon   *trayIcon_;   //!< Объявляем объект иконки приложения для трея

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
                         qint8 typeDatagram, QString state, QString msg, QString name);

private slots:
    void onProcessDatagram();
    void on_startServer_triggered();

    /**
     * @brief onIconActivated обработка принятия сигнала от события нажатия
     * на иконку приложения в трей
     * @param reason
     */
    void onIconActivated(QSystemTrayIcon::ActivationReason reason);

protected:
    /**
     * @brief closeEvent Виртуальная функция родительского класса в нашем классе
     * переопределяется для изменения поведения приложения,
     * чтобы оно сворачивалось в трей, когда мы этого хотим
     * @param event событие
     */
    void closeEvent(QCloseEvent *event);
};

#endif // SERVER_H
