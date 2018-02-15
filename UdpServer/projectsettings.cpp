#include "projectsettings.h"

ProjectSettings *ProjectSettings::instance_ = 0;

ProjectSettings *ProjectSettings::getInstance()
{
    if (!instance_) {
        instance_ = new ProjectSettings();
    }
    return instance_;
}

void ProjectSettings::setUserAuthorizationLogin(QString login)
{
    authorizationLogin_ = login;
}

void ProjectSettings::setUserAuthorizationPassword(QString password)
{
    authorizationPassword_ = password;
}

void ProjectSettings::setUserAuthorizationIpAdress(QString ip)
{
    ipAdress_ = ip;
}

void ProjectSettings::setUserAuthorizationPort(int port)
{
    port_ = port;
}

void ProjectSettings::setUserRegistrationLogin(QString login)
{
    registrationLogin_ = login;
}

void ProjectSettings::setUserRegistrationNickName(QString nickName)
{
    registrationNickName_ = nickName;
}

void ProjectSettings::setUserRegistrationPassword(QString password)
{
    registrationPassword_ = password;
}

