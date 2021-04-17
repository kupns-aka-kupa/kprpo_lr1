#include "init.hpp"

QLatin1String operator""_QL(const char *p, size_t s)
{
    return QLatin1String(p, s);
}

const auto MIGRATIONS_SQL = QLatin1String(R"(
create table migrations(id text primary key);
   )");

const auto ConnectionString = QString("User ID=postgres;Password=postgres;Host=localhost;Port=5432;Database=\"%0\";")
        .arg(PROJECT_NAME);

const auto migrationsTable = QString("migrations");

QSqlError initDb()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setDatabaseName(PROJECT_NAME);
    db.setUserName("postgres");
    db.setPassword("postgres");

    if (!db.open())
        return db.lastError();

    QSqlQuery q;
    QStringList tables = db.tables();

    if(!tables.contains(migrationsTable, Qt::CaseInsensitive)
       && !q.exec(MIGRATIONS_SQL)) return q.lastError();

    auto model = QSqlTableModel();
    model.setTable(migrationsTable);
    model.select();

    QDirIterator it(MIGRATIONS_DIR, {"*.sql"}, QDir::Files);

    for(int i = 0; it.hasNext(); i++)
    {
        auto next = it.next();
        if(model.record(i).value("id").toString() == QFileInfo(next).baseName()) continue;

        QFile f(next);
        QTextStream in(&f);

        if (!f.open(QFile::ReadOnly | QIODevice::Text)) break;
        if(!q.exec(in.readAll().simplified())) break;

        QSqlRecord record = model.record();
        record.setValue("id", QFileInfo(f.fileName()).baseName());
        model.insertRecord(-1, record);
    }

    model.submitAll();
    return q.lastError();
}
