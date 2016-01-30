local db = MySQL("localhost",  "tribiqtrunk", "password", "tribiqtrunk");
print(db.version());
print(db.exec_scalar("select count(*) from tribiq_content;"));
local qry = db.exec_query("select * from tribiq_content;");
local col_count = qry.col_count();
while(qry.next_row()){
	for(local i=0; i < col_count; ++i){
		print1(qry.col_value(i));
		print1("|");
	}
	print1("\n");
}
print("row_as_array");
qry._curr_row = -1;
while(qry.next_row()){
	foreach(v in qry.row_as_array()){
		print1(v);
		print1("|");
	}
	print1("\n");
}

local stmt = db.prepare("select * from tribiq_content;");
qry = stmt.execute();
col_count = qry.col_count();
while(qry.next_row()){
	for(local i=0; i < col_count; ++i){
		print1(qry.col_value(i));
		print1("|");
	}
	print1("\n");
}
print("row_as_array");
qry._curr_row = -1;
while(qry.next_row()){
	foreach(v in qry.row_as_array()){
		print1(v);
		print1("|");
	}
	print1("\n");
}

db.close();