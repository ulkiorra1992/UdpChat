#ifndef REGISTRATIONUSERS_H
#define REGISTRATIONUSERS_H

#include <QDialog>

namespace Ui {
class RegistrationUsers;
}

class RegistrationUsers : public QDialog
{
    Q_OBJECT

public:
    explicit RegistrationUsers(QWidget *parent = 0);
    ~RegistrationUsers();

    QString getUserLogin() const;
    QString getUserNickName() const;
    QString getUserPassword() const;
    QString getServerIp() const;

private:
    Ui::RegistrationUsers *ui;
};

#endif // REGISTRATIONUSERS_H
