#include "init.hpp"

QLatin1String operator""_QL(const char *p, size_t s)
{
    return QLatin1String(p, s);
}

QVariant addBook(QSqlQuery &q, const QString &title, int year, const QVariant &authorId,
             const QVariant &genreId, int rating)
{
    q.addBindValue(title);
    q.addBindValue(year);
    q.addBindValue(authorId);
    q.addBindValue(genreId);
    q.addBindValue(rating);
    q.exec();
    return q.lastInsertId();
}

QVariant addDict(QSqlQuery &q, const QString &name)
{
    q.addBindValue(name);
    q.exec();
    return q.lastInsertId();
}

QVariant addAuthor(QSqlQuery &q, const QString &name, QDate birthdate)
{
    q.addBindValue(name);
    q.addBindValue(birthdate);
    q.exec();
    return q.lastInsertId();
}

QVariant addIssue(QSqlQuery &q, const QVariant &book_id,
                  const QVariant &reader_id, QDate birthdate)
{
    q.addBindValue(reader_id);
    q.addBindValue(book_id);
    q.addBindValue(birthdate);
    q.exec();
    return q.lastInsertId();
}

QVariant addReader(
        QSqlQuery &q,
        const QString &name,
        QDate birthdate,
        const QString &phone,
        const QString &email)
{
    q.addBindValue(name);
    q.addBindValue(birthdate);
    q.addBindValue(phone);
    q.addBindValue(email);
    q.exec();
    return q.lastInsertId();
}

const auto BOOKS_SQL = QLatin1String(R"(
create table books(
    id serial primary key,
    title varchar,
    author_id integer,
    genre_id integer,
    year integer,
    rating integer,
    CONSTRAINT fk_genre
        FOREIGN KEY(genre_id)
            REFERENCES genres(id),
    CONSTRAINT fk_author
        FOREIGN KEY(author_id)
            REFERENCES authors(id));
    )");

const auto AUTHORS_SQL =  QLatin1String(R"(
create table authors(
    id serial primary key,
    name varchar,
    birthdate date);
    )");

const auto GENRES_SQL = QLatin1String(R"(
    create table genres(id serial primary key, name varchar)
    )");

const auto STATUSES_SQL = QLatin1String(R"(
    create table statuses(id serial primary key, name varchar)
    )");

const auto INSERT_AUTHOR_SQL = QLatin1String(R"(
    insert into authors(name, birthdate) values(?, ?)
    )");

const auto INSERT_BOOK_SQL = QLatin1String(R"(
    insert into books(title, year, author_id, genre_id, rating)
                      values(?, ?, ?, ?, ?)
    )");

const auto READERS_SQL = QLatin1String(R"(
create table readers(
    id serial primary key,
    phone text,
    email text,
    name varchar,
    birthdate date);
   )");

const auto ISSUES_SQL = QLatin1String(R"(
create table issues(
    id serial primary key,
    book_id integer,
    reader_id integer,
    date date,
    constraint fk_book
        foreign key(book_id)
            references books(id),
    constraint fk_reader
        foreign key(reader_id)
            references readers(id));
   )");

const auto INSERT_GENRE_SQL = QLatin1String(R"(
    insert into genres(name) values(?)
    )");

const auto INSERT_ISSUE_SQL = QLatin1String(R"(
    insert into issues(book_id, reader_id, date) values(?, ?, ?)
    )");

const auto INSERT_STATUS_SQL = QLatin1String(R"(
    insert into statuses(name) values(?)
    )");

const auto INSERT_READER_SQL = QLatin1String(R"(
    insert into readers(name, birthdate, phone, email) values(?, ?, ?, ?)
    )");

const auto ConnectionString = QString("User ID=postgres;Password=postgres;Host=localhost;Port=5432;Database=\"%0\";")
        .arg(PROJECT_NAME);

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
    q.exec( "create extension if not exists \"uuid-ossp\";");

    if(!tables.contains("statuses", Qt::CaseInsensitive)
       && !q.exec(STATUSES_SQL)) return q.lastError();

    if(!tables.contains("genres", Qt::CaseInsensitive)
       && !q.exec(GENRES_SQL)) return q.lastError();

    if(!tables.contains("readers", Qt::CaseInsensitive)
       && !q.exec(READERS_SQL)) return q.lastError();

    if(!tables.contains("authors", Qt::CaseInsensitive)
       && !q.exec(AUTHORS_SQL)) return q.lastError();

    if (!tables.contains("books", Qt::CaseInsensitive)
    && !q.exec(BOOKS_SQL)) return q.lastError();

    if(!tables.contains("issues", Qt::CaseInsensitive)
       && !q.exec(ISSUES_SQL)) return q.lastError();

    if (!q.prepare(INSERT_AUTHOR_SQL))
        return q.lastError();
    QVariant asimovId = addAuthor(q, QLatin1String("Isaac Asimov"), QDate(1920, 2, 1));
    QVariant greeneId = addAuthor(q, QLatin1String("Graham Greene"), QDate(1904, 10, 2));
    QVariant pratchettId = addAuthor(q, QLatin1String("Terry Pratchett"), QDate(1948, 4, 28));

    if (!q.prepare(INSERT_READER_SQL))
        return q.lastError();
    auto tommyId = addReader(q, QLatin1String("Tommy Haydn"), QDate(1988, 7, 7), QLatin1String("+1-202-555-0136 "), QLatin1String("qmahad.2g@opakenak.com"));
    addReader(q, QLatin1String("Noah Madlyn"), QDate(2002, 7, 15), QLatin1String("+1-202-555-0162"), QLatin1String("3emam.al@miekering.buzz"));
    addReader(q, QLatin1String("Xander Meaghan"), QDate(2005, 3, 28), QLatin1String("+1-202-555-0113"), QLatin1String("dlil.h010u@soundsgoodtomepromotions.com"));

    if (!q.prepare(INSERT_GENRE_SQL))
        return q.lastError();
    QVariant sfiction = addDict(q, QLatin1String("Science Fiction"));
    QVariant fiction = addDict(q, QLatin1String("Fiction"));
    QVariant fantasy = addDict(q, QLatin1String("Fantasy"));

    if (!q.prepare(INSERT_STATUS_SQL))
        return q.lastError();
    addDict(q, QLatin1String("Busy"));
    addDict(q, QLatin1String("Return"));
    addDict(q, QLatin1String("Ready"));

    if (!q.prepare(INSERT_BOOK_SQL))
        return q.lastError();
    auto foundationId = addBook(q, QLatin1String("Foundation"), 1951, asimovId, sfiction, 3);
    addBook(q, QLatin1String("Foundation and Empire"), 1952, asimovId, sfiction, 4);
    addBook(q, QLatin1String("Second Foundation"), 1953, asimovId, sfiction, 3);
    addBook(q, QLatin1String("Foundation's Edge"), 1982, asimovId, sfiction, 3);
    addBook(q, QLatin1String("Foundation and Earth"), 1986, asimovId, sfiction, 4);
    addBook(q, QLatin1String("Prelude to Foundation"), 1988, asimovId, sfiction, 3);
    addBook(q, QLatin1String("Forward the Foundation"), 1993, asimovId, sfiction, 3);
    addBook(q, QLatin1String("The Power and the Glory"), 1940, greeneId, fiction, 4);
    addBook(q, QLatin1String("The Third Man"), 1950, greeneId, fiction, 5);
    addBook(q, QLatin1String("Our Man in Havana"), 1958, greeneId, fiction, 4);
    addBook(q, QLatin1String("Guards! Guards!"), 1989, pratchettId, fantasy, 3);
    addBook(q, QLatin1String("Night Watch"), 2002, pratchettId, fantasy, 3);
    addBook(q, QLatin1String("Going Postal"), 2004, pratchettId, fantasy, 3);

    if (!q.prepare(INSERT_ISSUE_SQL))
        return q.lastError();
    addIssue(q, tommyId, foundationId, QDate::currentDate());

    return QSqlError();
}
