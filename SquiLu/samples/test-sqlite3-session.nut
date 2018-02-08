auto db = SQLite3("session.db");
db.exec_dml("drop table if exists test;");
db.exec_dml("create table if not exists test(id integer primary key, name text);");

class MySQLite3Session
{
	_db_src = null;
	_db2save = null;
	_dbsession = null;
	_dbfn2save = null;
	_stmt = null;
	
	MySQLite3Session(db, dbfn2save)
	{
		_db_src = db;
		_dbsession = SQLite3Session(db);
		_dbfn2save = dbfn2save;
		_newSession();
		_db2save = SQLite3(dbfn2save);
		_db2save.exec_dml("create table if not exists changesets(id integer primary key, day datetime not null default CURRENT_TIMESTAMP, changeset blob);");
		_stmt = _db2save.prepare("insert into changesets(changeset) values(?)");
	}

	function _newSession()
	{
		if(_dbsession) _dbsession.xdelete();
		_dbsession = SQLite3Session(_db_src);
		_dbsession.attach();
	}
	
	function save()
	{
		auto changeset = _dbsession.changeset();
		if(changeset && changeset.len())
		{
			_stmt.bind_blob(1, changeset);
			_stmt.step();
			_stmt.reset();
			_newSession();
		}
		return changeset;
	}
	
	function apply(db, cset)
	{
		return _dbsession.apply(db, cset);
	}
}

auto dbsession = new MySQLite3Session(db, "changeset.db");


db.exec_dml("insert into test values(1, 'dad')");

auto stmt = db.prepare("select * from test");
while(stmt.next_row())
{
	print(stmt.col(0), stmt.col(1));
}

auto changeset = dbsession.save();
print(changeset.len());



auto dbm = SQLite3(":memory:");
dbm.exec_dml("drop table if exists test;");
dbm.exec_dml("create table if not exists test(id integer primary key, name text);");

dbsession.apply(dbm, changeset);

auto stmtm = dbm.prepare("select * from test");
while(stmtm.next_row())
{
	print(stmtm.col(0), stmtm.col(1));
}
