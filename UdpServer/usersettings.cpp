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

bool UserSettings::setRegistrationUser()
{
// ============ Для отображения русских букв в файле Setting.ini ============ //
    QTextCodec *codecForLocaleName = QTextCodec::codecForName("Windows-1251");
    QTextCodec::setCodecForLocale(codecForLocaleName);
//

   QSettings settings(QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/setting.ini"),
                             QSettings::IniFormat);
   settings.setIniCodec(codecForLocaleName);
// Проверка зарегистрирован ли пользователь на сервере
   if (settings.contains("RegistrationLogin/" + ProjectSettings::getInstance()->getUserRegistrationLogin().toString())) {
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

QString UserSettings::setAuthorizationUser()
{
    QString nickName;
    QSettings settings(QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/setting.ini"),
                       QSettings::IniFormat);

    if (settings.contains("RegistrationLogin/" + ProjectSettings::getInstance()->getUserAuthorizationLogin().toString())) {
        settings.beginGroup("RegistrationPassword");
        QString regPassword = settings.value(ProjectSettings::getInstance()->getUserAuthorizationLogin().toString(),
                                             ProjectSettings::getInstance()->getUserRegistrationPassword().toString()).toString();
        settings.endGroup();

        if (ProjectSettings::getInstance()->getUserAuthorizationPassword() == regPassword) {
//            settings.beginGroup("AuthorizationIpAddres");
//            settings.setValue(ProjectSettings::getInstance()->getUserAuthorizationLogin().toString(),
//                              ProjectSettings::getInstance()->getUserAuthorizationIpAdress());
//            settings.endGroup();

            settings.beginGroup("RegistrationLogin");
            nickName = settings.value(ProjectSettings::getInstance()->getUserAuthorizationLogin().toString(),
                                      ProjectSettings::getInstance()->getUserRegistrationNickName()).toString();
            return nickName;
        } else {
            return nickName;
        }
    } else {
        return nickName;
    }
}
