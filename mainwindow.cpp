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

    _model = new QSqlRelationalTableModel(_ui->bookTable);
    _model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    _model->setTable("books");

    _authorIdx = _model->fieldIndex("author");
    _genreIdx = _model->fieldIndex("genre");

    _model->setRelation(_authorIdx, QSqlRelation("authors", "id", "name"));
    _model->setRelation(_genreIdx, QSqlRelation("genres", "id", "name"));

    _model->setHeaderData(_authorIdx, Qt::Horizontal, tr("Author Name"));
    _model->setHeaderData(_genreIdx, Qt::Horizontal, tr("Genre"));
    _model->setHeaderData(_model->fieldIndex("title"),
                          Qt::Horizontal, tr("Title"));
    _model->setHeaderData(_model->fieldIndex("year"), Qt::Horizontal, tr("Year"));
    _model->setHeaderData(_model->fieldIndex("rating"),
                          Qt::Horizontal, tr("Rating"));

    if (!_model->select()) {
        showError(_model->lastError());
        return;
    }

    _ui->bookTable->setModel(_model);
    _ui->bookTable->setItemDelegate(new BookDelegate(_ui->bookTable));
    _ui->bookTable->setColumnHidden(_model->fieldIndex("id"), true);
    _ui->bookTable->setSelectionMode(QAbstractItemView::SingleSelection);

    _ui->authorEdit->setModel(_model->relationModel(_authorIdx));
    _ui->authorEdit->setModelColumn(
            _model->relationModel(_authorIdx)->fieldIndex("name"));

    _ui->genreEdit->setModel(_model->relationModel(_genreIdx));
    _ui->genreEdit->setModelColumn(
            _model->relationModel(_genreIdx)->fieldIndex("name"));

    _ui->bookTable->horizontalHeader()->setSectionResizeMode(
            _model->fieldIndex("rating"),
            QHeaderView::ResizeToContents);

    QDataWidgetMapper *mapper = new QDataWidgetMapper(this);
    mapper->setModel(_model);
    mapper->setItemDelegate(new BookDelegate(this));
    mapper->addMapping(_ui->titleEdit, _model->fieldIndex("title"));
    mapper->addMapping(_ui->yearEdit, _model->fieldIndex("year"));
    mapper->addMapping(_ui->authorEdit, _authorIdx);
    mapper->addMapping(_ui->genreEdit, _genreIdx);
    mapper->addMapping(_ui->ratingEdit, _model->fieldIndex("rating"));

    connect(_ui->bookTable->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            mapper,
            &QDataWidgetMapper::setCurrentModelIndex
    );

    _ui->bookTable->setCurrentIndex(_model->index(0, 0));
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
    delete _ui;
}

