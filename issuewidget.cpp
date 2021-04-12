#include "issuewidget.hpp"
#include "ui_issuewidget.h"

IssueWidget::IssueWidget(QWidget *parent)
    : QGroupBox(parent)
    , _ui(new Ui::IssueWidget)
    , _mapper(new QDataWidgetMapper(this))
{
    _ui->setupUi(this);
    _model = new QSqlRelationalTableModel(_ui->issuesTable);
    _model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    _model->setTable("issues");
    _model->setFilter("closed = false");

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
    _ui->issuesTable->setColumnHidden(_model->fieldIndex("closed"), true);
    _ui->issuesTable->setSelectionMode(QAbstractItemView::SingleSelection);

    _ui->dateEdit->setMinimumDate(QDate::currentDate());

    _ui->bookEdit->setModel(_model->relationModel(_bookIdx));
    _ui->bookEdit->setModelColumn(
            _model->relationModel(_bookIdx)->fieldIndex("title"));

    _ui->readerEdit->setModel(_model->relationModel(_readerIdx));
    _ui->readerEdit->setModelColumn(
            _model->relationModel(_readerIdx)->fieldIndex("name"));

    _ui->issuesTable->horizontalHeader()->setSectionResizeMode(
            _model->fieldIndex("date"),
            QHeaderView::ResizeToContents);

    _mapper->setModel(_model);
    _mapper->setItemDelegate(new QSqlRelationalDelegate(_ui->issuesTable));
    _mapper->addMapping(_ui->dateEdit, _model->fieldIndex("date"));
    _mapper->addMapping(_ui->bookEdit, _bookIdx);
    _mapper->addMapping(_ui->readerEdit, _readerIdx);

    connect(_ui->issuesTable->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            _mapper,
            &QDataWidgetMapper::setCurrentModelIndex
    );
    connect(_ui->addPushButton, &QPushButton::clicked, this, &IssueWidget::add);
    connect(_ui->closeIssuePushButton, &QPushButton::clicked, this, &IssueWidget::closeIssue);
    connect(_ui->submitPushButton, &QPushButton::clicked, this, &IssueWidget::submit);

    _ui->issuesTable->setCurrentIndex(_model->index(0, 0));
}

void IssueWidget::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                          "Error initializing database: " + err.text());
}

void IssueWidget::add()
{
    int row = _mapper->currentIndex();

    if (_mapper->submit()) {
        _model->insertRecord(row, _model->record(row));
        _mapper->setCurrentIndex(row);
    } else {
        _model->insertRow(0);
    }
}

const auto UPDATE_STATUS_SQL = QLatin1String(R"(
update books set status_id = ? where title = ?;
    )");

void updateStatus(QSqlQuery &q, const QVariant &status_id, const QVariant &id)
{
    q.addBindValue(status_id);
    q.addBindValue(id);
    q.exec();
}

void IssueWidget::closeIssue()
{
    int row = _mapper->currentIndex();

    auto record = _model->record(row);
    auto status_id = 3;
    auto book_id = record.value(_bookIdx);

    QSqlQuery q;
    if (q.prepare(UPDATE_STATUS_SQL))
        updateStatus(q, status_id, book_id);

    record.setValue("closed", true);
    _model->setRecord(row, record);
    if (q.lastError().type() != QSqlError::NoError) {
        QMessageBox::warning(this, tr("Issues Table"),
                             tr("The database reported an error: %1")
                                     .arg(_model->lastError().text()));
    }

    submit();

    if (!_model->select()) {
        showError(_model->lastError());
        return;
    }
    _ui->issuesTable->setCurrentIndex(_model->index(0, 0));
}

void IssueWidget::submit()
{
    _model->database().transaction();
    if (_model->submitAll()) {
        _model->database().commit();
        _ui->issuesTable->setCurrentIndex(_model->index(0, 0));
    } else {
        _model->database().rollback();
        QMessageBox::warning(this, tr("Issues Table"),
                             tr("The database reported an error: %1")
                                     .arg(_model->lastError().text()));
    }
}

IssueWidget::~IssueWidget()
{
    delete _ui;
}
