#include "issuewidget.hpp"
#include "ui_issuewidget.h"

IssueWidget::IssueWidget(QWidget *parent)
    : QGroupBox(parent)
    , _ui(new Ui::IssueWidget)
{
    _ui->setupUi(this);
    _model = new QSqlRelationalTableModel(_ui->issuesTable);
    _model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    _model->setTable("issues");

    _bookIdx = _model->fieldIndex("book_id");
    _readerIdx = _model->fieldIndex("reader_id");

    _model->setRelation(_bookIdx, QSqlRelation("books", "id", "title"));
    _model->setRelation(_readerIdx, QSqlRelation("readers", "id", "name"));

    _model->setHeaderData(_bookIdx, Qt::Horizontal, tr("Book Name"));
    _model->setHeaderData(_readerIdx, Qt::Horizontal, tr("Reader"));
    _model->setHeaderData(_model->fieldIndex("date"), Qt::Horizontal, tr("Date"));

    if (!_model->select()) {
        showError(_model->lastError());
        return;
    }

    _ui->issuesTable->setModel(_model);
    _ui->issuesTable->setColumnHidden(_model->fieldIndex("id"), true);
    _ui->issuesTable->setSelectionMode(QAbstractItemView::SingleSelection);

    _ui->readerEdit->setModel(_model->relationModel(_bookIdx));
    _ui->readerEdit->setModelColumn(
            _model->relationModel(_bookIdx)->fieldIndex("title"));

    _ui->bookEdit->setModel(_model->relationModel(_readerIdx));
    _ui->bookEdit->setModelColumn(
            _model->relationModel(_readerIdx)->fieldIndex("name"));

    _ui->issuesTable->horizontalHeader()->setSectionResizeMode(
            _model->fieldIndex("date"),
            QHeaderView::ResizeToContents);

    QDataWidgetMapper *mapper = new QDataWidgetMapper(this);
    mapper->setModel(_model);
    mapper->addMapping(_ui->statusEdit, _model->fieldIndex("date"));
    mapper->addMapping(_ui->bookEdit, _bookIdx);
    mapper->addMapping(_ui->readerEdit, _readerIdx);

    connect(_ui->issuesTable->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            mapper,
            &QDataWidgetMapper::setCurrentModelIndex
    );

    _ui->issuesTable->setCurrentIndex(_model->index(0, 0));
}

void IssueWidget::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                          "Error initializing database: " + err.text());
}

IssueWidget::~IssueWidget()
{
    delete _ui;
}
