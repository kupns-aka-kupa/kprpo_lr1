#include "tableeditor.hpp"

TableEditor::TableEditor(const QString &tableName, QWidget *parent)
        : QWidget(parent)
        , _model(new QSqlTableModel(this))
{
    _model->setTable(tableName);
    _model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    _model->select();

    QSqlQuery q;
    q.exec(QString(R"(
    select column_name, ordinal_position
      from information_schema.columns
    where table_schema = 'public'
      and table_name   = '%0';
    )").arg(_model->tableName()));

    while (q.next()) {
        QString name = q.value(0).toString();
        int section = q.value(1).toInt() - 1;
        _model->setHeaderData(section, Qt::Horizontal, name);
    }

    QTableView *view = new QTableView;
    view->setModel(_model);
    view->resizeColumnsToContents();

    _submitButton = new QPushButton(tr("Submit"));
    _submitButton->setDefault(true);
    _revertButton = new QPushButton(tr("&Revert"));
    _quitButton = new QPushButton(tr("Quit"));

    _buttonBox = new QDialogButtonBox(Qt::Vertical);
    _buttonBox->addButton(_submitButton, QDialogButtonBox::ActionRole);
    _buttonBox->addButton(_revertButton, QDialogButtonBox::ActionRole);
    _buttonBox->addButton(_quitButton, QDialogButtonBox::RejectRole);

    connect(_submitButton, &QPushButton::clicked, this, &TableEditor::submit);
    connect(_revertButton, &QPushButton::clicked, _model, &QSqlTableModel::revertAll);
    connect(_quitButton, &QPushButton::clicked, this, &TableEditor::close);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(view);
    mainLayout->addWidget(_buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("Cached Table"));
}

void TableEditor::submit()
{
    _model->database().transaction();
    if (_model->submitAll()) {
        _model->database().commit();
    } else {
        _model->database().rollback();
        QMessageBox::warning(this, tr("Cached Table"),
                             tr("The database reported an error: %1")
                                     .arg(_model->lastError().text()));
    }
}