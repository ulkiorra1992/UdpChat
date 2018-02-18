#include "registrationusers.h"
#include "ui_registrationusers.h"

RegistrationUsers::RegistrationUsers(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegistrationUsers)
{
    ui->setupUi(this);
    setWindowTitle("Регистрация");

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

RegistrationUsers::~RegistrationUsers()
{
    delete ui;
}

QString RegistrationUsers::getUserLogin() const
{
    return ui->leUserLogin->text();
}

QString RegistrationUsers::getUserNickName() const
{
    return ui->leUserName->text();
}

QString RegistrationUsers::getUserPassword() const
{
    return ui->leUserPassword->text();
}

QString RegistrationUsers::getServerIp() const
{
    return ui->leServerIp->text();
}

