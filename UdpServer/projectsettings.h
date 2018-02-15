#ifndef PROJECTSETTINGS_H
#define PROJECTSETTINGS_H

#include "core_global.h"

#include <QString>
#include <QVariant>
#include <QHostAddress>

class CORESHARED_EXPORT ProjectSettings
{
public:
    /**
     * @brief getInstance синглтон.
     * @return возвращает указатель на себя.
     */
    static ProjectSettings *getInstance();

    void setUserAuthorizationLogin(QString login);
    void setUserAuthorizationPassword(QString password);
    void setUserAuthorizationIpAdress(QString ip);
    void setUserAuthorizationPort(int port);

    void setUserRegistrationLogin(QString login);
    void setUserRegistrationNickName(QString nickName);
    void setUserRegistrationPassword(QString password);

    QVariant getUserAuthorizationLogin() {return authorizationLogin_; }
    QVariant getUserAuthorizationPassword() { return authorizationPassword_; }
    QVariant getUserAuthorizationIpAdress() {return ipAdress_; }
    QVariant getUserAuthorizationPort() {return port_ ;}

    QVariant getUserRegistrationLogin() {return registrationLogin_; }
    QVariant getUserRegistrationNickName() {return registrationNickName_; }
    QVariant getUserRegistrationPassword() { return registrationPassword_; }

private:
    ProjectSettings() {

    };
    virtual ~ProjectSettings() {

    };

    static ProjectSettings *instance_;

    QString authorizationLogin_;
    QString authorizationPassword_;
    QString ipAdress_;
    int port_;

    QString registrationLogin_;
    QString registrationNickName_;
    QString registrationPassword_;
};

#endif // PROJECTSETTINGS_H
