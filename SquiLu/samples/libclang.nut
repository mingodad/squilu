function libclangParse2sqlite(dbname, source_file_name, ...)
{
	auto db = SQLite3(dbname);

	db.exec_dml([==[
		CREATE TABLE IF NOT EXISTS function_file (
			id INTEGER PRIMARY KEY,
			file_name VARCHAR UNIQUE NOT NULL
		);
		]==]);

	db.exec_dml([==[
		CREATE TABLE IF NOT EXISTS function_declaration (
			id INTEGER PRIMARY KEY,
			name VARCHAR NOT NULL,
			ftype VARCHAR NOT NULL,
			return_type VARCHAR NOT NULL,
			linkage_type INTEGER,
			file_id INTEGER NOT NULL REFERENCES function_file(id),
			line INTEGER,
			col INTEGER,
			UNIQUE(file_id, name, line, col)
		);
		]==]);

	db.exec_dml([==[
		CREATE INDEX IF NOT EXISTS function_declaration_name_file_idx ON function_declaration(name, file_id);
		]==]);

	db.exec_dml([==[
		CREATE TABLE IF NOT EXISTS function_param (
			id INTEGER PRIMARY KEY,
			function_id INTEGER NOT NULL REFERENCES function_declaration(id),
			name VARCHAR,
			type VARCHAR NOT NULL,
			kind_id INTEGER,
			UNIQUE(function_id, name)
		);
		]==]);

	db.exec_dml([==[
		CREATE TABLE IF NOT EXISTS function_calling (
			id INTEGER PRIMARY KEY,
			caller_id INTEGER NOT NULL REFERENCES function_declaration(id),
			callee_id INTEGER NOT NULL REFERENCES function_declaration(id),
			file_id INTEGER NOT NULL REFERENCES function_file(id),
			line INTEGER,
			col INTEGER,
			UNIQUE(file_id, caller_id, callee_id, line, col)
		);
		]==]);

	db.exec_dml([==[
		CREATE INDEX IF NOT EXISTS function_calling_caller_idx ON function_calling(caller_id);
		CREATE INDEX IF NOT EXISTS function_calling_callee_idx ON function_calling(callee_id);
		]==]);

	auto stmt_get_file_id = db.prepare("SELECT id  FROM function_file  WHERE file_name = ?;");
	auto stmt_get_func_decl_id = db.prepare("SELECT id  FROM function_declaration  WHERE file_id = ? AND name=? AND line=? AND col=?;");
	auto stmt_insert_file = db.prepare("INSERT OR IGNORE INTO function_file (file_name) VALUES (?);");
	auto stmt_insert_function = db.prepare("INSERT OR IGNORE INTO function_declaration (name, ftype, return_type, linkage_type, file_id, line, col) VALUES (?, ?, ?, ?, ?, ?, ?);");
	auto stmt_insert_call = db.prepare("INSERT OR IGNORE INTO function_calling (caller_id, callee_id, file_id, line, col) VALUES (?, ?, ?, ?, ?);");
	auto stmt_insert_param = db.prepare("INSERT OR IGNORE INTO function_param (function_id, name, type, kind_id) VALUES (?, ?, ?, ?);");

	local function cleanBackwardPath(path)
	{
		while(path.find("/../") > 0)
		{
			path = path.gsub("/[^/.]+/%.%.", "");
		}
		return path;
	}

	local function add_funcfile(file_name)
	{
		local id = null;
		file_name = cleanBackwardPath(file_name);
		
		stmt_get_file_id.bind(1, file_name);

		if (stmt_get_file_id.next_row()) {
			id = stmt_get_file_id.col(0);
		}
		stmt_get_file_id.reset();
		if(id) {
			return id;
		}

		stmt_insert_file.bind(1, file_name);

		local rc = stmt_insert_file.step();
		if (rc != db.SQLITE_DONE) {
			print("Can't insert: %d : %s\n", rc, db.errmsg());
			os.exit(-1);
		}

		stmt_insert_file.reset();
		return db.last_row_id();
	}

	local function add_funcdecl(file_name, line, col, func_name, func_type, return_type, linkage_type)
	{
		local file_id = add_funcfile(file_name);
		stmt_insert_function.bind(1, func_name);
		stmt_insert_function.bind(2, func_type);
		stmt_insert_function.bind(3, return_type);
		stmt_insert_function.bind(4, linkage_type);
		stmt_insert_function.bind(5, file_id);
		stmt_insert_function.bind(6, line);
		stmt_insert_function.bind(7, col);

		local rc = stmt_insert_function.step();
		if (rc != db.SQLITE_DONE) {
			print("Can't insert: %d : %s\n", rc, db.errmsg());
			os.exit(-1);
		}

		stmt_insert_function.reset();
	}

	local function get_func_decl_id(file_id, func_name, line, col)
	{
		local id = null;
		stmt_get_func_decl_id.bind(1, file_id);
		stmt_get_func_decl_id.bind(2, func_name);
		stmt_get_func_decl_id.bind(3, line);
		stmt_get_func_decl_id.bind(4, col);

		if (stmt_get_func_decl_id.next_row()) {
			id = stmt_get_func_decl_id.col(0);
		}
		stmt_get_func_decl_id.reset();
		return id;
	}

	local function add_funcparam(file_name, line, col, func_name, param_name, param_type, type_kind)
	{
		local file_id = add_funcfile(file_name);
		local func_id = get_func_decl_id(file_id, func_name, line, col);
		stmt_insert_param.bind(1, func_id);
		stmt_insert_param.bind(2, param_name);
		stmt_insert_param.bind(3, param_type);
		stmt_insert_param.bind(4, type_kind);

		local rc = stmt_insert_param.step();
		if (rc != db.SQLITE_DONE) {
			print("Can't insert: %d : %s\n", rc, db.errmsg());
			os.exit(-1);
		}

		stmt_insert_param.reset();
	}

	local function add_funccall(file_name, line, col, from_func, from_line, from_col, to_func_file, to_func_line, to_func_col, to_func)
	{
		local file_id = add_funcfile(file_name);
		local from_func_id = get_func_decl_id(file_id, from_func, from_line, from_col);
		
		local to_file_id = add_funcfile(to_func_file);
		local to_func_id = get_func_decl_id(to_file_id, to_func, to_func_line, to_func_col);
		
		stmt_insert_call.bind(1, from_func_id);
		stmt_insert_call.bind(2, to_func_id);
		stmt_insert_call.bind(3, file_id);
		stmt_insert_call.bind(4, line);
		stmt_insert_call.bind(5, col);

		local rc = stmt_insert_call.step();
		if (rc != db.SQLITE_DONE) {
			print("Can't insert: %d : %s\n", rc, db.errmsg());
			os.exit(-1);
		}

		stmt_insert_call.reset();
	}

	db.exec_dml("BEGIN;");

	auto libclang = new LibClang();


	local function libclangVisitor(...)
	{
		//print("Hello!");
		//print(vargv.join("\t"));

		//try
		{
			local vtype = vargv[0];
			switch(vtype)
			{
				case "CallExpr":
					//vtype, file_name, line, col, from_func, from_line, from_col, to_func_file, to_func_line, to_func_col, to_func
					add_funccall(vargv[1], vargv[2], vargv[3], vargv[4], vargv[5], vargv[6], vargv[7], vargv[8], vargv[9], vargv[10]);
				break;
				case "FuncDecl":
					//vtype, file_name, line, col, func_name, func_type, return_type, linkage_type
					add_funcdecl(vargv[1], vargv[2], vargv[3], vargv[4], vargv[5], vargv[6], vargv[7]);
				break;
				case "FuncParam":
					//vtype, file_name, line, col, func_name, param_name, param_type, type_kind
					add_funcparam(vargv[1], vargv[2], vargv[3], vargv[4], vargv[5], vargv[6], vargv[7]);
				break;
			}
		}
		//catch(e)
		{
			//print(e);
		}
	}

	//libclang.parseTranslationUnit(libclangVisitor, source_file_name, "-I.", "-I/home/mingo/local/clang-3.6/include");
	auto call_args = [libclang, libclangVisitor, source_file_name];
	foreach(p in vargv)
	{
		call_args.append(p);
	}
	libclang.parseTranslationUnit.acall(call_args);

	libclang.close();

	db.exec_dml([==[
	CREATE VIEW IF NOT EXISTS "function_calling_list_view" AS
	SELECT
		a."id",
		d.name AS caller,
		c.name AS callee,
		b.file_name,
		a."line",
		a."col",
		a."caller_id",
		a."callee_id",
		a."file_id"
	FROM "function_calling" AS a
	LEFT JOIN "function_file" AS b ON a."file_id" = b."id"
	LEFT JOIN "function_declaration" AS c ON a."callee_id" = c."id"
	LEFT JOIN "function_declaration" AS d ON a."caller_id" = d."id";

	CREATE VIEW IF NOT EXISTS "function_declaration_list_view" AS
	SELECT
		a."id",
		a."name",
		a."ftype",
		a."return_type",
		a."linkage_type",
		b.file_name,
		a."line",
		a."col",
		a."file_id"
	FROM "function_declaration" AS a
	LEFT JOIN "function_file" AS b ON a."file_id" = b."id";

	CREATE VIEW IF NOT EXISTS "function_param_list_view" AS
	SELECT
		a."id",
		b."name" AS function_name,
		a."name" AS param_name,
		a."type",
		a."kind_id",
		a."function_id"
	FROM "function_param" AS a
	LEFT JOIN "function_declaration" AS b ON a."function_id" = b."id";

	CREATE VIEW IF NOT EXISTS function_calling_count_view AS
	SELECT b.id, b.name, COUNT(*) AS calls, c.file_name
	FROM function_calling a
		LEFT JOIN function_declaration b
			ON a.callee_id = b.id
		LEFT JOIN function_file c
			ON b.file_id = c.id
	GROUP BY a.callee_id
	ORDER BY 3 DESC, 2 ASC;
	]==]);

/*
	db.exec_dml([==[
	CREATE VIEW IF NOT EXISTS functions_of_interest_view AS
	WITH
	source_file AS(
		SELECT id FROM function_file WHERE file_name = 'SOURCE_FILE_NAME'
	)
	SELECT caller_id as fid FROM function_calling a, source_file as b WHERE a.file_id = b.id
	UNION
	SELECT callee_id as fid FROM function_calling a, source_file as b WHERE a.file_id = b.id
	UNION
	SELECT a.id as fid FROM function_declaration a, source_file as b WHERE a.file_id = b.id;
	]==].gsub("SOURCE_FILE_NAME", source_file_name));

	db.exec_dml([==[
	-- remove function parameter not in use
	DELETE FROM function_param 
	WHERE function_id NOT IN(
		SELECT fid FROM functions_of_interest_view
	);

	-- remove functio_declaration not in use
	DELETE FROM function_declaration 
	WHERE id NOT IN(
		SELECT fid FROM functions_of_interest_view
	);

	-- remove function_file not in use
	DELETE FROM function_file 
	WHERE id NOT IN(
		SELECT DISTINCT file_id FROM function_declaration
	);
	]==]);
*/
	db.exec_dml("COMMIT;");

	stmt_insert_param.finalize();
	stmt_insert_call.finalize();
	stmt_insert_function.finalize();
	stmt_get_func_decl_id.finalize();
	stmt_insert_file.finalize();
	stmt_get_file_id.finalize();
	db.close();
}

//libclangParse2sqlite("c2sqlite.db", "c2sqlite.c", "-I.", "-I/home/mingo/local/clang-3.6/include");
