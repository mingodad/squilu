auto db = SQLite3(":memory:");

db.exec_dml("create table test(id integer primary key, value text)");
auto stmt_insert = db.prepare("insert into test(value) values(?)");

auto max_loop = 10000;
auto start_time;

start_time = os.clock();
for(auto i=0; i < max_loop; ++i)
{
	stmt_insert.bind(1, "dad" + i);
	stmt_insert.step();
	stmt_insert.reset();
}
print("Time spent", os.clock() - start_time);
db.exec_dml("delete from test");

start_time = os.clock();
for(auto i=0; i < max_loop; ++i)
{
	{
		auto stmt_sr = db.stmt_scope_reset(stmt_insert);
		stmt_insert.bind(1, "dad" + i);
		stmt_insert.step();
	}
}
print("Time spent", os.clock() - start_time);
db.exec_dml("delete from test");

start_time = os.clock();
for(auto i=0; i < max_loop; ++i)
{
	stmt_insert.bind_exec("dad" + i);
}
print("Time spent", os.clock() - start_time);
db.exec_dml("delete from test");

start_time = os.clock();
db.exec_dml("begin");
for(auto i=0; i < max_loop; ++i)
{
	stmt_insert.bind(1, "dad" + i);
	stmt_insert.step();
	stmt_insert.reset();
}
print("Time spent", os.clock() - start_time);
db.exec_dml("delete from test");
db.exec_dml("commit");

start_time = os.clock();
db.exec_dml("begin");
for(auto i=0; i < max_loop; ++i)
{
	{
		auto stmt_sr = db.stmt_scope_reset(stmt_insert);
		stmt_insert.bind(1, "dad" + i);
		stmt_insert.step();
	}
}
print("Time spent", os.clock() - start_time);
db.exec_dml("delete from test");
db.exec_dml("commit");

start_time = os.clock();
db.exec_dml("begin");
for(auto i=0; i < max_loop; ++i)
{
	stmt_insert.bind_exec("dad" + i);
}
print("Time spent", os.clock() - start_time);
db.exec_dml("delete from test");
db.exec_dml("commit");
