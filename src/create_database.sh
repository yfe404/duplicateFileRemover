#!/bin/bash

# args : $1 => database name
# args : $2 => table name

echo -e "create table $2  (filepath varchar(255) primary key, filename varchar(255), lastmodified varchar(255), md5sum varchar(255));\n" | /usr/bin/sqlite3 $1


