#include "bookwidget.hpp"
#include "ui_bookwidget.h"

BookWidget::BookWidget(QWidget *parent)
    : QGroupBox(parent)
    , _ui(new Ui::BookWidget)
{
    _ui->setupUi(this);

    _model = new QSqlRelationalTableModel(_ui->bookTable);
    _model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    _model->setTable("books");

    _authorIdx = _model->fieldIndex("author_id");
    _genreIdx = _model->fieldIndex("genre_id");

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
}

void BookWidget::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                          "Error initializing database: " + err.text());
}

BookWidget::~BookWidget()
{
    delete _ui;
}
