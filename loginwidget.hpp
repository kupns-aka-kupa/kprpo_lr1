#ifndef LOGIN_HPP
#define LOGIN_HPP

#include <QGroupBox>
#include <QString>
#include <QtSql>
#include <QMessageBox>

namespace Ui {
class LoginWidget;
}

class LoginWidget : public QGroupBox
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

signals:
    void verified();

private slots:
    void onLoginClicked();

private:
    const QLatin1String _salt = QLatin1String("$1$w0EJdcPo");
    QLatin1String _query = QLatin1String(R"(
select true from operators
 where login = '%0' and
password = crypt('%1', '%2'))");

    Ui::LoginWidget *_ui;
};

#endif // LOGIN_HPP
