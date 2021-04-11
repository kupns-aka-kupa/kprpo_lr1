#ifndef ISSUE_WIDGET_HPP
#define ISSUE_WIDGET_HPP

#include <QMessageBox>
#include <QGroupBox>
#include <QtSql>
#include <QDataWidgetMapper>

namespace Ui {
class IssueWidget;
}

class IssueWidget : public QGroupBox
{
    Q_OBJECT

public:
    explicit IssueWidget(QWidget *parent = nullptr);
    ~IssueWidget();

private slots:
    void add();
    void submit();

private:
    void showError(const QSqlError &err);

private:
    Ui::IssueWidget *_ui;
    QSqlRelationalTableModel *_model;
    QDataWidgetMapper *_mapper;
    int _bookIdx, _readerIdx;
};

#endif // ISSUE_WIDGET_HPP
