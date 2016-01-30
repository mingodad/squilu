//local db = PostgreSQL("host=localhost dbname=ourbiz user=ourbiz password=ourbiz2");
//local db = PostgreSQL("host=localhost port=7432 dbname=mingo user=mingo password=password");
//local db = PostgreSQL("port=7432 dbname=odoo");
local db = PostgreSQL("port=7432 dbname=odoo");
local start = os.clock();
print(db.version());
print(db.exec_scalar("select count(*) from pg_type;"));
//local qry = db.exec_query("select * from pg_type;");
//local qry = db.exec_query("select * from pg_views where schemaname='public';");
//::regclass
local qry = db.exec_query("select pgc.oid, pgt.* from pg_tables as pgt join pg_class as pgc on pgt.tablename = pgc.relname where schemaname='public';");
//local qry = db.exec_query("select tablename from pg_tables where schemaname='public';");
//local qry = db.exec_query("SELECT * FROM information_schema.columns WHERE table_schema = 'public'  AND table_name   = 'account_account'");
local col_count = qry.col_count();
for(local i=0; i < col_count; ++i){
	print1(qry.col_name(i));
	print1("|");
}
print1("\n");
while(qry.next_row()){
	for(local i=0; i < col_count; ++i){
		print1(qry.col_value(i));
		print1("|");
	}
	print1("\n");
}

qry._curr_row = -1;
while(qry.next_row()){
	foreach(v in qry.row_as_array()){
		print1(v);
		print1("|");
	}
	print1("\n");
}
qry.close();

db.close();
print("Total time", os.clock() - start);