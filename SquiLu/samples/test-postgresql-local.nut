//local db = PostgreSQL("host=localhost dbname=mingo user=mingo port=7432");
local db = PostgreSQL("host=/tmp dbname=mingo user=mingo port=7432");


for(local i=0; i < 10000; ++i)
{
    local qry = db.exec_query("select t from test where i=1;");
    qry.next_row();
    //print(qry.col_value(0));
    qry.close();
}

db.close();