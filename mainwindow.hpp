#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QMessageBox>
#include <QtSql>

#include "init.hpp"
#include "bookdelegate.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *_ui;

private slots:
    void about();

private:
    void showError(const QSqlError &err);
    QSqlRelationalTableModel *_model;
    int _authorIdx, _genreIdx;

    void createMenuBar();
};
#endif // MAINWINDOW_HPP
