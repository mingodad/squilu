auto db = SQLite3(":memory:");
db.exec_dml("create table if not exists test(id integer primary key, data blob);");
auto stmt_insert = db.prepare("insert into test(data) values(?)");
for(auto i=0; i < 10; ++i)
{
	stmt_insert.bind_zeroblob(1, 2048);
	stmt_insert.step();
	stmt_insert.reset();
}
print(db.exec_get_one("select count(*) from test"));
auto dbblob = db.blob_open("main", "test", "data", 1, 1);

print(dbblob);
print(dbblob.bytes());
dbblob.write("Hello");
print(dbblob.read(5));