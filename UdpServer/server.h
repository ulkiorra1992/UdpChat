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

private slots:
    void onProcessDatagram();

    void on_startServer_triggered();

protected:

};

#endif // SERVER_H
