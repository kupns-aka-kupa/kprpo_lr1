#include "loginwidget.hpp"
#include "ui_loginwidget.h"

LoginWidget::LoginWidget(QWidget *parent)
    : QGroupBox(parent)
    , _ui(new Ui::LoginWidget)
{
    _ui->setupUi(this);
    connect(_ui->loginPushButton, &QPushButton::clicked, this, &LoginWidget::onLoginClicked);
}

void LoginWidget::onLoginClicked()
{
    QString login = _ui->loginLineEdit->text();
    QString password = _ui->passwordLineEdit->text();

    auto s = _query.arg(login.trimmed(), password.trimmed(), _salt);
     QSqlQuery query(s);

    if(query.next())
    {
        emit LoginWidget::verified();
    }
    else QMessageBox::warning(this,"Login", "Username and password is not correct");
}

LoginWidget::~LoginWidget()
{
    delete _ui;
}
