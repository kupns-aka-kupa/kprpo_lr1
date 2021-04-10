#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

    model = new QSqlRelationalTableModel(ui->bookTable);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("books");

    authorIdx = model->fieldIndex("author");
    genreIdx = model->fieldIndex("genre");

    model->setRelation(authorIdx, QSqlRelation("authors", "id", "name"));
    model->setRelation(genreIdx, QSqlRelation("genres", "id", "name"));

    model->setHeaderData(authorIdx, Qt::Horizontal, tr("Author Name"));
    model->setHeaderData(genreIdx, Qt::Horizontal, tr("Genre"));
    model->setHeaderData(model->fieldIndex("title"),
                         Qt::Horizontal, tr("Title"));
    model->setHeaderData(model->fieldIndex("year"), Qt::Horizontal, tr("Year"));
    model->setHeaderData(model->fieldIndex("rating"),
                         Qt::Horizontal, tr("Rating"));

    if (!model->select()) {
        showError(model->lastError());
        return;
    }

    ui->bookTable->setModel(model);
    ui->bookTable->setItemDelegate(new BookDelegate(ui->bookTable));
    ui->bookTable->setColumnHidden(model->fieldIndex("id"), true);
    ui->bookTable->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->authorEdit->setModel(model->relationModel(authorIdx));
    ui->authorEdit->setModelColumn(
           model->relationModel(authorIdx)->fieldIndex("name"));

    ui->genreEdit->setModel(model->relationModel(genreIdx));
    ui->genreEdit->setModelColumn(
           model->relationModel(genreIdx)->fieldIndex("name"));

    ui->bookTable->horizontalHeader()->setSectionResizeMode(
            model->fieldIndex("rating"),
            QHeaderView::ResizeToContents);

    QDataWidgetMapper *mapper = new QDataWidgetMapper(this);
    mapper->setModel(model);
    mapper->setItemDelegate(new BookDelegate(this));
    mapper->addMapping(ui->titleEdit, model->fieldIndex("title"));
    mapper->addMapping(ui->yearEdit, model->fieldIndex("year"));
    mapper->addMapping(ui->authorEdit, authorIdx);
    mapper->addMapping(ui->genreEdit, genreIdx);
    mapper->addMapping(ui->ratingEdit, model->fieldIndex("rating"));

    connect(ui->bookTable->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            mapper,
            &QDataWidgetMapper::setCurrentModelIndex
    );

    ui->bookTable->setCurrentIndex(model->index(0, 0));
    createMenuBar();
}

void MainWindow::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                          "Error initializing database: " + err.text());
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
    delete ui;
}

