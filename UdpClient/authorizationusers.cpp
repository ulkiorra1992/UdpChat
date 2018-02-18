#include "authorizationusers.h"
#include "ui_authorizationusers.h"

AuthorizationUsers::AuthorizationUsers(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AuthorizationUsers)
{
    ui->setupUi(this);
    setWindowTitle("Авторизация");
// Создаем строку для регулярного выражения
    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
// Создаем регулярное выражение с применением строки, как повторяющегося элемента
    QRegExp ipRegex ("^" + ipRange
                         + "\\." + ipRange
                         + "\\." + ipRange
                         + "\\." + ipRange + "$");
// Создаем Валидатор регулярного выражения с применением созданного регулярного выражения
    QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);
// Устанавливаем Валидатор на QLineEdit
    ui->leServerIp->setValidator(ipValidator);
}

AuthorizationUsers::~AuthorizationUsers()
{
    delete ui;
}

QString AuthorizationUsers::getServerIp() const
{
    return ui->leServerIp->text();
}

QString AuthorizationUsers::getUserLogin() const
{
    return ui->leUserLogin->text();
}

QString AuthorizationUsers::getUserPassword() const
{
    return ui->leUserPassword->text();
}
