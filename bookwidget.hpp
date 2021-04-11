#ifndef BOOK_WIDGET_HPP
#define BOOK_WIDGET_HPP

#include <QGroupBox>
#include <QSqlError>
#include <QMessageBox>
#include <QtSql>
#include <QModelIndex>
#include <QSqlRelationalDelegate>
#include <QDataWidgetMapper>

namespace Ui {
class BookWidget;
}

class BookWidget : public QGroupBox
{
    Q_OBJECT

public:
    explicit BookWidget(QWidget *parent = nullptr);
    ~BookWidget();

private:
    void showError(const QSqlError &err);

private:
    Ui::BookWidget *_ui;
    QSqlRelationalTableModel *_model;
    int _authorIdx, _genreIdx, _statusIdx;
};

#endif // BOOK_WIDGET_HPP
