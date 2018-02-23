#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QAction>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QModelIndex>
#include <QTimer>

#include "inquirystate.h"

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
    QTimer *pTimer_;
    InquiryState *state_;

    QMap <QString, QString> users_;
    qint8 type_;
    QString login_;
    QString nickName_;
    QString password_;
    QString serverIp_;
    QString userLogin_;
    QColor colorOutMsg_;
    QColor colorInMsg_;

    void usersData();
    void usersListWidget(QString user);


private slots:
    void onProcessDatagram();
    void on_tbSend_clicked();
    void onShowMessage(QString message);    //!< Всплывающее информативное сообщение с текстом принятого сообщения.
    void onMessageClicked();                //!< При клике по всплывающему сообщению открывается окно чата.
    void on_Quit_triggered();
    void onListWidgetUser(QModelIndex index);
    void onConnectToServer();
    void onServerState(bool state);
    void onTextChanged();

    void on_Mountains_triggered();
    void on_NoStyle_triggered();
    void on_font_triggered();
    void on_ColorFontOut_triggered();
    void on_ColorFontIn_triggered();

protected:
    /**
     * @brief closeEvent Виртуальная функция родительского класса в нашем классе
     * переопределяется для изменения поведения приложения,
     * чтобы оно сворачивалось в трей, когда мы этого хотим
     * @param event событие
     */
    void closeEvent(QCloseEvent *event);
    void keyReleaseEvent(QKeyEvent * event);
};

#endif // CLIENT_H
