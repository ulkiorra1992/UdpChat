#ifndef AUTHORIZATIONUSERS_H
#define AUTHORIZATIONUSERS_H

#include <QDialog>

namespace Ui {
class AuthorizationUsers;
}

class AuthorizationUsers : public QDialog
{
    Q_OBJECT

public:
    explicit AuthorizationUsers(QWidget *parent = 0);
    ~AuthorizationUsers();

    QString getServerIp() const;
    QString getUserLogin() const;
    QString getUserPassword() const;

private:
    Ui::AuthorizationUsers *ui;
};

#endif // AUTHORIZATIONUSERS_H
