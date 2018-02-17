#ifndef USERSETTINGS_H
#define USERSETTINGS_H

#include <QObject>
#include <QMap>

class UserSettings : public QObject
{

    Q_OBJECT

public:
    UserSettings(QObject *parent = 0);

    /**
     * @brief getRegistrationUser регистрация нового пользователя и возвращение
     * результата регистрации успех/не успех.
     * @return
     */
    static bool getRegistrationUser();
    /**
     * @brief getAuthorizationUser авторизация зарегистрированного пользователя
     * и возвращение его имени.
     * @return
     */
    static QString getAuthorizationUser(QString login, QString password);
    /**
     * @brief getUserAdress возвращает ip адресс авторизовавшегося пользователя по его логину
     * @param login логин
     * @return
     */
    static QString getUserAdress(QString login);
    /**
     * @brief getUserPort вовзращает порт авторизовавшегося пользователя по его логину
     * @param login догин
     * @return
     */
    static int getUserPort(QString login);

};

#endif // USERSETTINGS_H
