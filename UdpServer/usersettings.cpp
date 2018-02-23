#include "usersettings.h"
#include "projectsettings.h"

#include <QDir>
#include <QSettings>
#include <QTextCodec>
#include <QCoreApplication>

UserSettings::UserSettings(QObject *parent) :
    QObject(parent)
{

}

bool UserSettings::getRegistrationUser()
{
// ============ Для отображения русских букв в файле Setting.ini ============ //
    QTextCodec *codecForLocaleName = QTextCodec::codecForName("CP866");
    QTextCodec::setCodecForLocale(codecForLocaleName);
//

   QSettings settings(QDir::toNativeSeparators(QCoreApplication::applicationDirPath()
                                               + "/setting.ini"), QSettings::IniFormat);
   settings.setIniCodec(codecForLocaleName);
// Проверка зарегистрирован ли пользователь на сервере
   if (settings.contains("RegistrationLogin/" +
                         ProjectSettings::getInstance()->getUserRegistrationLogin().toString())) {
        return false;
   } else {
       settings.beginGroup("RegistrationLogin");
       settings.setValue(ProjectSettings::getInstance()->getUserRegistrationLogin().toString(),
                         ProjectSettings::getInstance()->getUserRegistrationNickName());
       settings.endGroup();

       settings.beginGroup("RegistrationPassword");
       settings.setValue(ProjectSettings::getInstance()->getUserRegistrationLogin().toString(),
                         ProjectSettings::getInstance()->getUserRegistrationPassword());
       settings.endGroup();
       return true;
   }
}

QString UserSettings::getAuthorizationUser(QString login, QString password)
{
    QString nickName;
    QSettings settings(QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/setting.ini"),
                       QSettings::IniFormat);

    if (settings.contains("RegistrationLogin/" + login)) {
        settings.beginGroup("RegistrationPassword");
        QString regPassword = settings.value(login).toString();
        settings.endGroup();

        if (password == regPassword) {
            settings.beginGroup("RegistrationLogin");
            nickName = settings.value(login).toString();
            settings.endGroup();

            settings.beginGroup(ProjectSettings::getInstance()->getUserAuthorizationLogin().toString());
            settings.setValue("IP", ProjectSettings::getInstance()->getUserAuthorizationIpAdress());
            settings.setValue("Port", ProjectSettings::getInstance()->getUserAuthorizationPort());
            settings.endGroup();

            return nickName;
        } else {
            return nickName;
        }
    } else {
        return nickName;
    }
}

QString UserSettings::getUserAdress(QString login)
{
    QString ipAdress;
    QSettings settings(QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/setting.ini"),
                       QSettings::IniFormat);
    if (settings.contains(login + "/IP")) {
        ipAdress = settings.value(login + "/IP").toString();
        return ipAdress;
    }
    return ipAdress;
}

int UserSettings::getUserPort(QString login)
{
    int port;
    QSettings settings(QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/setting.ini"),
                       QSettings::IniFormat);
    if (settings.contains(login + "/Port")) {
        port = settings.value(login + "/Port").toInt();
        return port;
    }
    return port;
}
