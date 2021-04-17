create extension if not exists pgcrypto;

create table operators(
    login text primary key,
    password text);

insert into operators (login, password) values ('operator', crypt('Q12345q', '$1$w0EJdcPo'));

