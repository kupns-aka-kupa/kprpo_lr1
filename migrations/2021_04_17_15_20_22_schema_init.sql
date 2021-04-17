
create table genres(
    id serial primary key,
    name varchar);

create table statuses(
    id serial primary key,
    name varchar);

create table authors(
    id serial primary key,
    name varchar,
    birthdate date);

create table books(
    id serial primary key,
    title varchar,
    author_id integer,
    genre_id integer,
    status_id integer not null default 3,
    year integer,
    rating integer,
    constraint fk_status
        foreign key(status_id)
            references statuses(id),
    constraint fk_genre
        foreign key(genre_id)
            references genres(id),
    constraint fk_author
        foreign key(author_id)
            references authors(id));

create table readers(
    id serial primary key,
    phone text,
    email text,
    name varchar,
    birthdate date);

create table issues(
    id serial primary key,
    book_id integer not null,
    reader_id integer not null,
    date date not null,
    closed boolean not null default false,
    constraint fk_book
        foreign key(book_id)
            references books(id),
    constraint fk_reader
        foreign key(reader_id)
            references readers(id));

create or replace function on_issues_create() returns trigger as
$BODY$
BEGIN
    update books set status_id = 1 where id = new.book_id;
    return new;
END;
$BODY$
    language plpgsql;

drop trigger if exists trigger_issue_create on issues;

create trigger trigger_issue_create
    after insert on issues
    for each row
execute procedure on_issues_create();
