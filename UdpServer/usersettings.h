#ifndef USERSETTINGS_H
#define USERSETTINGS_H

#include <QObject>

class UserSettings : public QObject
{

    Q_OBJECT

public:
    UserSettings(QObject *parent = 0);

    static bool setRegistrationUser();            //!<
    static QString setAuthorizationUser();
};

#endif // USERSETTINGS_H
