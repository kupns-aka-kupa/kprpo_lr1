#ifndef BOOK_WIDGET_HPP
#define BOOK_WIDGET_HPP

#include <QGroupBox>
#include <QSqlError>
#include <QMessageBox>
#include <QtSql>

#include "bookdelegate.hpp"

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
    int _authorIdx, _genreIdx;
};

#endif // BOOK_WIDGET_HPP
