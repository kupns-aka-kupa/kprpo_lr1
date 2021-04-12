#ifndef TABLE_EDITOR_HPP
#define TABLE_EDITOR_HPP

#include <QDialog>
#include <QHBoxLayout>
#include <QTableView>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlTableModel>
#include <QPushButton>
#include <QDialogButtonBox>

class TableEditor : public QWidget
{
    Q_OBJECT

public:
    explicit TableEditor(const QString &tableName, QWidget *parent = nullptr);

private slots:
    void submit();

private:
    QPushButton *_submitButton;
    QPushButton *_revertButton;
    QPushButton *_quitButton;
    QDialogButtonBox *_buttonBox;
    QSqlTableModel *_model;
};

#endif //TABLE_EDITOR_HPP
