#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QMessageBox>
#include <QAction>
#include <QMenuBar>
#include <QtSql>

#include "init.hpp"
#include "bookwidget.hpp"
#include "loginwidget.hpp"
#include "issuewidget.hpp"
#include "tableeditor.hpp"

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
    void tabChanged(int index);
    void load();
    void about();

private:
    int _booksIndex;
    BookWidget *_books;
    LoginWidget *_login;

private:
    void showError(const QSqlError &err);

    void createMenuBar();
};
#endif // MAINWINDOW_HPP
