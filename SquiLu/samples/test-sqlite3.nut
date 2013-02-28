local db = SQLite3(":memory:");
print(db);

db.create_function("multiply3",3,function(ctx,a,b,c){ctx.result_double(a*b*c);});

local sql = "select 1.2*2.5*3.6;";
local sql_squilu = "select multiply3(1.2,2.5,3.6);";

local stmt = db.prepare(sql);
local stmt_squilu = db.prepare(sql_squilu);

print(db.exec_get_one(sql_squilu));

local count = 100000;

local now = os.clock();
for(local i=0; i<count; ++i){
	local val = db.exec_get_one(sql_squilu);
}
print("SquiLu function took:", os.clock() -now);

now = os.clock();
for(local i=0; i<count; ++i){
	local val = db.exec_get_one(sql);
}
print("SQL function took:", os.clock() -now);

stmt_squilu.reset();
stmt_squilu.step()
print(stmt_squilu.col(0));

now = os.clock();
for(local i=0; i<count; ++i){
	stmt_squilu.reset();
	stmt_squilu.step()
	local val = stmt_squilu.col(0);
}
print("SquiLu SQL prepared function took:", os.clock() -now);

now = os.clock();
for(local i=0; i<count; ++i){
	stmt.reset();
	stmt.step()
	local val = stmt.col(0);
}
print("SQL prepared function took:", os.clock() -now);

stmt.finalize();
stmt_squilu.finalize();

db.close();