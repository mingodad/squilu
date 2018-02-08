auto db = SQLite3(":memory:");

db.exec_dml("create table test(id integer primary key, value text)");
auto stmt_insert = db.prepare("insert into test(value) values(?)");

stmt_insert.bind_exec("dad");

auto stmt = db.prepare("select * from test");
while(stmt.next_row()) print(stmt.col(0), stmt.col(1));

auto transaction = db.transaction();
print(transaction);

stmt_insert.bind_exec("car");
while(stmt.next_row()) print(stmt.col(0), stmt.col(1));

transaction.rollback();
while(stmt.next_row()) print(stmt.col(0), stmt.col(1));


transaction = db.transaction();
print(transaction);
stmt_insert.bind_exec("car2");
while(stmt.next_row()) print(stmt.col(0), stmt.col(1));

transaction.commit();
while(stmt.next_row()) print(stmt.col(0), stmt.col(1));

{
    auto transaction3 = db.transaction();
    print(transaction3);
    auto stmt_sr = db.stmt_scope_reset(stmt_insert);
    stmt_insert.bind(1, "car3");
    stmt_insert.step();
    stmt_insert.reset(); //redundant to test double reset trough stmt_scope_reset
    //stmt_insert.bind_exec("car3");
    while(stmt.next_row()) print(stmt.col(0), stmt.col(1));
}

print("after block transaction");
while(stmt.next_row()) print(stmt.col(0), stmt.col(1));

{
    auto transaction4 = db.transaction();
    print(transaction4);
    auto stmt_sr = db.stmt_scope_reset(stmt_insert);
    stmt_insert.bind(1, "car4");
    stmt_insert.step();
    //stmt_insert.bind_exec("car4");
    while(stmt.next_row()) print(stmt.col(0), stmt.col(1));
}

print("End result");
while(stmt.next_row()) print(stmt.col(0), stmt.col(1));
