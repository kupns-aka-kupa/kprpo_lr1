#ifndef INIT_HPP
#define INIT_HPP
#include <QtSql>
#include <QFileInfo>
#include <QSqlDriver>

QSqlError initDb(const QSqlDatabase &);

#endif //INIT_HPP
