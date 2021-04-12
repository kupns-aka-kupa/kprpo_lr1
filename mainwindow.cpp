#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);

    if (!QSqlDatabase::drivers().contains("QSQLITE"))
        QMessageBox::critical(
                this,
                "Unable to load database",
                "This demo needs the SQLITE driver"
        );

    QSqlError err = initDb();
    if (err.type() != QSqlError::NoError) {
        showError(err);
        return;
    }

    QWidget *issues = new IssueWidget(this);
    _ui->tabWidget->addTab(issues, tr("Issues"));

    _books = new BookWidget(this);
    _ui->tabWidget->addTab(_books, tr("Books"));
    _booksIndex = _ui->tabWidget->indexOf(_books);

    QWidget *authors = new TableEditor("authors", this);
    _ui->tabWidget->addTab(authors, tr("Authors"));

    QWidget *readers = new TableEditor("readers", this);
    _ui->tabWidget->addTab(readers, tr("Readers"));

    connect(_ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::tabChanged);
    createMenuBar();
}

void MainWindow::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                          "Error initializing database: " + err.text());
}

void MainWindow::tabChanged(int index)
{
    if (index == _booksIndex) _books->reload();
}

void MainWindow::createMenuBar()
{
    QAction *quitAction = new QAction(tr("&Quit"), this);
    QAction *aboutAction = new QAction(tr("&About"), this);
    QAction *aboutQtAction = new QAction(tr("&About Qt"), this);

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(quitAction);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);

    connect(quitAction, &QAction::triggered, this, &MainWindow::close);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::about);
    connect(aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Books"),
                       tr("<p>The <b>Books</b> example shows how to use Qt SQL classes "
                          "with a model/view framework."));
}

MainWindow::~MainWindow()
{
    delete _ui;
}

