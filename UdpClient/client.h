#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QAction>
#include <QCloseEvent>
#include <QSystemTrayIcon>

namespace Ui {
class Client;
}

class Client : public QMainWindow
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = 0);
    ~Client();

private slots:
    void on_Registration_triggered();
    void on_Authorization_triggered();
    /**
     * @brief onIconActivated обработка принятия сигнала от события нажатия
     * на иконку приложения в трей
     * @param reason
     */
    void onIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    Ui::Client *ui;
    QUdpSocket *udpSocket_;
    QSystemTrayIcon   *trayIcon_;   //!< Объявляем объект иконки приложения для трея

    qint8 type_;
    QString login_;
    QString nickName_;
    QString password_;
    QString serverIp_;

    void usersData();

private slots:
    void onProcessDatagram();
    void on_tbSend_clicked();
    void onShowMessage(QString message);    //!< Всплывающее информативное сообщение с текстом принятого сообщения.
    void onMessageClicked();                //!< При клике по всплывающему сообщению открывается окно чата.

protected:
    /**
     * @brief closeEvent Виртуальная функция родительского класса в нашем классе
     * переопределяется для изменения поведения приложения,
     * чтобы оно сворачивалось в трей, когда мы этого хотим
     * @param event событие
     */
    void closeEvent(QCloseEvent *event);
};

#endif // CLIENT_H
