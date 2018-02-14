#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>
#include <QUdpSocket>

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

private:
    Ui::Client *ui;
    QUdpSocket *udpSocket_;

private slots:
    void onProcessDatagram();
    void on_tbSend_clicked();
};

#endif // CLIENT_H
