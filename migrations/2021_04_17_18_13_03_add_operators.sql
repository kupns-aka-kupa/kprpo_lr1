create extension if not exists pgcrypto;

create table operators(
    verified text primary key,
    password text);

insert into operators (verified, password) values ('operator', crypt('Q12345q', '$1$w0EJdcPo'));

