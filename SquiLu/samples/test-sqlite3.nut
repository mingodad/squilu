local db = SQLite3(":memory:");
print(db);

class MultiplyDecimal3 {
	first = null;
	second = null;
	thirdy = null;
	constructor(){
		first = Decimal(0);
		second = Decimal(0);
		thirdy = Decimal(0);
	}
	function reset(){
		first.set(0);
		second.set(0);
		thirdy.set(0);
	}
	function doIt(){
		return (first * second * thirdy).tostring();
	}
}

local function multiply3_MultiplyDecimal3(ctx,a,b,c){
	//print(ctx.user_data());
	//local m3 = Multiply3();
	local m3 = ctx.user_data();
	m3.reset();
	m3.first.set(a);
	m3.second.set(b);
	m3.thirdy.set(c);
	ctx.result_text(m3.doIt());
	//ctx.result_double(a*b*c);
}

class Multiply3 {
	first = null;
	second = null;
	thirdy = null;
	constructor(){
		reset();
	}
	function reset(){
		first = second = thirdy = 0.0;
	}
	function doIt(){
		return first * second * thirdy;
	}
}

local function multiply3_Multiply3(ctx,a,b,c){
	//print(ctx.user_data());
	//local m3 = Multiply3();
	local m3 = ctx.user_data();
	m3.reset();
	m3.first = a;
	m3.second = b;
	m3.thirdy = c;
	ctx.result_double(m3.doIt());
	//ctx.result_double(a*b*c);
}

local function multiply3(ctx,a,b,c){
	//print(ctx.user_data());
	ctx.result_double(a*b*c);
}

local function sq_multiply3(a,b,c){
	//print(ctx.user_data());
	return (a*b*c);
}

local function showall(ctx,...){
	for(local i = 0; i< vargv.len(); i++)
	{
		::print("varparam "+i+" = "+vargv[i]+"\n");
	}
	//print(ctx.user_data());
	ctx.result_null();
}

db.create_function("showall",-1, showall);
db.exec_get_one("select showall(1,'two', 3, 'four')");
db.exec_dml("create table tr(id integer primary key, name text);");
db.exec_dml("create trigger tr_trigger before insert on tr begin select showall(new.id, new.name); end;");
db.exec_dml("insert into tr(id,name) values(1, 'dad'), (2, 'car');");
//db.exec_get_one("select showall(*) from tr");

//db.create_function("multiply3",3, multiply3_MultiplyDecimal3, MultiplyDecimal3());
//db.create_function("multiply3",3, multiply3_Multiply3, Multiply3());
db.create_function("multiply3",3, multiply3);

local sql = "select 1.2*2.5*3.6;";
local sql_squilu = "select multiply3(1.2,2.5,3.6);";

local stmt = db.prepare(sql);
local stmt_squilu = db.prepare(sql_squilu);

print(db.exec_get_one(sql_squilu));
print(db.exec_get_one(sql));

local count = 100000;

local now = os.clock();
print("top", getstacktop(), dostring("return 1.2*2.5*3.6;", true));
for(local i=0; i<count; ++i){
	local val = dostring("return 1.2*2.5*3.6;", true);
}
print("top", getstacktop());
print("SquiLu function eval took:", os.clock() -now);

now = os.clock();
for(local i=0; i<count; ++i){
	local val = db.exec_get_one(sql_squilu);
}
print("SquiLu sqlite function took:", os.clock() -now);

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
	local val = sq_multiply3(1.2,2.5,3.6);
}
print("SquiLu function took:", os.clock() -now);

now = os.clock();
for(local i=0; i<count; ++i){
	local val = (1.2*2.5*3.6);
}
print("SquiLu direct calc took:", os.clock() -now);

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

db.exec_dml("create table if not exists test_slice(value text)");
db.exec_dml("insert into test_slice values('value text')");
stmt = db.prepare("select * from test_slice");
if(stmt.next_row()) print("col_slice", stmt.col_slice(0, 2, 5));
stmt.finalize();


db.close();