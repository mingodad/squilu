local db = PostgreSQL("host=localhost dbname=ourbiz user=ourbiz password=ourbiz2");
print(db.version());
print(db.exec_scalar("select count(*) from pg_type;"));
local qry = db.exec_query("select * from pg_type;");
local col_count = qry.col_count();
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

db.close();