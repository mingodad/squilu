local db = SQLite3("file::memory:?cache=shared");
db.restore("ourbiz.db");
print(db.exec_get_one("select count(*) from orders;"));

local db2 = SQLite3("file::memory:?cache=shared");
print(db2.exec_get_one("select count(*) from orders;"));
db2.close();

db.backup("ourbiz-bkp.db");

db.close();