/*
 * Copyright (C) 2013 by Domingo Alvarez Duarte <mingodad@gmail.com>
 *
 * Licensed under GPLv3, see http://www.gnu.org/licenses/gpl.html.
 */
 
local globals = getroottable();
if(!globals.rawget("APP_CODE_FOLDER", false)) ::APP_CODE_FOLDER <- ".";
WIN32 <- os.getenv("WINDIR") != null;

math.srand(os.time());

//local AT_DEV_DBG=true;

//local APP_CODE_FOLDER = sqfs.currentdir();
//local EDIT_MD5_PASSWORD = md5("edit_user:r.dadbiz.es:okdoedit");
//local VIEW_MD5_PASSWORD = md5("view_user:r.dadbiz.es:okdoview");

class MySMTP  {
	boundary = null;
	smtp_server = null;
	smtp_port = null;
	smtp_user_name = null;
	smtp_user_passw = null;
	smtp_from = null;
	smtp_to = null;
	smtp_subject = null;
	smtp_message = null;
	attachments = null;
	_ssl = null;
	_quite = null;
	_fout = null;
	
	constructor(smtp, port){
		server(smtp, port);
		attachments = [];
		_quite = false;
	}
	function server(smtp, port) {
		smtp_server = smtp; 
		smtp_port = port;
		boundary = "_=_+-mixed-+19JK4720AB04PX483";
		_fout = file("MySMPT.log", "wb");
	}
	function login(user, passw) {
		smtp_user_name = user;
		smtp_user_passw = passw;
	}
	function from(pfrom) {smtp_from = pfrom;}
	function to(pto) {smtp_to = pto; }
	function subject(psubject) {smtp_subject = psubject; }
	function message(pmessage) {smtp_message = pmessage; }
	function attach(fn, mime) {
		attachments.push([fn, mime]);
	}
	function _write(str){
		_ssl.write(str);
		_fout.write(str);
	}
	function _read_line(expected_code){
		local result;
		while (true){
			result = _ssl.read();
			if(type(result) == "integer"){
				if (result < 0){ 
					throw(axtls.get_error(result));
				}
			}
			if(type(result) == "string"){
				//print(result);
				local response_code;
				result.gmatch("^(%d+)", function(m){
						response_code = m.tointeger();
						return false;
					});
				//print("Code check", expected_code, response_code);
				if(!response_code || (expected_code != response_code)){
					throw(format("Response code '%d' not equal to expected '%d'", 
						response_code, expected_code));
				}
				return true;
			}
			os.sleep(10);
		}
	}
	function _write_line(line){
		local end_line = "\r\n";
		local result = _write(line);
		_write(end_line);
		return result;
	}
	function _write_message(msg){
		local end_msg = "\r\n.\r\n";
		local result = _write(msg);
		_write(end_msg);
		return result;
	}
	function _getAttachemt(fn){
		local fd = file(fn, "rb");
		local fc = fd.read(fd.len());
		fd.close();
		return base64.encode(fc);
	}
	function send(){
		local client_sock = socket.tcp();
		client_sock.connect(smtp_server, smtp_port);

		local options = axtls.SSL_SERVER_VERIFY_LATER;
		local ssl_ctx = axtls.ssl_ctx(options, axtls.SSL_DEFAULT_CLNT_SESS);

		_ssl = ssl_ctx.client_new(client_sock.getfd());
		// check the return status
		local res = _ssl.handshake_status();
		if (res != axtls.SSL_OK) throw( axtls.get_error(res));
		
		_read_line(220);
		_write_line("ehlo " + smtp_user_name);
		_read_line(250);
		local credentials = base64.encode(format("\x00%s\x00%s", 
			smtp_user_name, smtp_user_passw));
		//print("credentials", credentials);
		_write_line(format("AUTH PLAIN %s", credentials));
		_read_line(235);
		_write_line(format("mail from: <%s>", smtp_from || smtp_user_name));
		_read_line(250);
		_write_line(format("rcpt to: <%s>", smtp_to));
		_read_line(250);
		_write_line("data");
		_read_line(354);

		local buf = format([==[
From: Tais Alvarez <%s>
To: Domingo <%s>
Subject: %s
]==], smtp_from || smtp_user_name, smtp_to, smtp_subject);
		_write(buf);
		local hasAttachment = attachments.len() > 0;
		if (hasAttachment){
			buf = format([==[
Content-Type: multipart/mixed; boundary="%s"

--%s
Content-Type: text/plain; charset="utf-8" 
Content-Transfer-Encoding: 8bit
]==], boundary, boundary);
			_write(buf);
		}
		buf = format([==[

%s
%s
]==], os.date(), smtp_message);
		_write(buf);
		if (hasAttachment){
			foreach( k,v in attachments) {
				buf = format([==[
--%s
Content-Type: %s; name="%s" 
Content-Transfer-Encoding: base64
Content-Disposition: attachment

%s
]==], boundary, v[1], v[0], _getAttachemt(v[0]));
				_write(buf);
			}
			buf = format("--%s--", boundary);
			_write(buf);
		}
		_write_message("");
		//print("Done !");
		_read_line(250);
		_write_line("quit");
		_read_line(221);
		_ssl.free();
		ssl_ctx.free();
		client_sock.close();
	}
}

function IntToDottedIP( intip )
{
	local octet = [0,0,0,0];
	for(local i=3; i >= 0; --i)
	{
		octet[i] = (intip & 0xFF).tostring();
		intip = intip >> 8;
	}
	return octet.concat(".");
}

if(!globals.rawget("gmFile", false)) ::gmFile <- blob();
if(!globals.rawget("__tplCache", false)) ::__tplCache <- {};

function getTemplate(fname, nocache){
	local mixBase = ::__tplCache.rawget(fname, false);
	if (!mixBase || nocache){
		local rfn = format("%s/%s", APP_CODE_FOLDER, fname);
		//debug_print("\n", rfn);
		try {
			mixBase = sqmix.loadfile(rfn);
		} catch(e){
			debug_print("\n", e);
		}
		::__tplCache[fname] <- mixBase;
	}
	return mixBase;
}

/*
if(!globals.rawget("__stmtCache", false)) ::__stmtCache <- {};
function getCachedStmt(db, stmt_key, sql_or_func){
	local stmt = ::__stmtCache.rawget(stmt_key, false);
	if (!stmt){
		//local db =checkCompaniesUkDB()
		local sql;
		if (type(sql_or_func) == "function") sql = sql_or_func();
		else sql = sql_or_func;
		//debug_print("\n", sql);
		stmt = ::db.prepare(sql);
		::__stmtCache.stmt_key <- stmt;
	}
	return stmt;
}
*/

function glob2Sql(v){
	if( v ) {
		v = v.gsub("*", "%%");
		v = v.gsub("%?", "_");
	}
	return v;
}

function unescapeHtml ( str ){
	if (str){
		return str.gsub("(&[^;]-;)", function(m){
				if (m == "&lt;") return "<";
				else if (m == "&gt;") return ">";
				else if (m == "&amp;") return "&";
				return m;
			});
	}
}

function escapeHtml ( str ){
	if (str){
		return str.gsub("([<>&])", function(m){
				if (m == "<") return "&lt;";
				else if (m == ">") return "&gt;";
				else if (m == "&") return "&amp;";
				return m;
			});
	}
}

function var2json(v){
	switch(type(v)){
		case "table":
			local result = [];
			foreach(k2, v2 in v) {
				result.push( format([==["%s":%s]==], k2, var2json(v2)));
			}
			return "{" + result.concat(",") + "}";
			break;
			
		case "array":
			local result = [];
			for(local i=0, len=v.len(); i<len; ++i) {
				result.push(var2json(v[i]));
			}
			return "[" + result.concat(",") + "]";
			break;
			
		case "integer":
		case "float":
			return v.tostring();
			break;

		case "bool":
			return v ? "true" : "false";
			break;

		case "null":
			return "null";
			break;

		default:
			return "\"" + v.tostring().replace("\"", "\\\"").replace("\n", "\\n") + "\"";
	}
	return "";
}

function json2var(json) {
	local vm = SlaveVM();
	local slave_func = "getTable";
	
	//debug_print(json, "\n");
	//convert new data from json to squilu table for merge
	vm.compilestring(slave_func, "return " + json);
	local tbl = vm.call(true, slave_func);
	return tbl;
}

function doSaveTableArrayToFD(ta, fd){
	local function dumpValue(val){
		local vtype = type(val);
		if(vtype == "string") fd.write(format("%q,\n", val));
		else if(vtype == "integer") fd.write(format("%d,\n", val));
		else if(vtype == "float") fd.write(format("%f,\n", val));
		else if(vtype == "bool") fd.write(format("%s,\n", val ? "true" : "false"));
		else if(vtype == "null") fd.write("null,\n");
		else throw format("Only string, integer, float, bool are supported ! (%s)", vtype);
	}
	local tatype = type(ta);
	if(tatype == "table"){
		fd.write(format("{\n"));
		foreach(k,v in ta){
			fd.write(format("[%q] = ", k));
			local vtype = type(v);
			if(vtype == "table" || vtype == "array"){
				doSaveTableArrayToFD(v, fd);
			}
			else
			{
				dumpValue(v);
			}
		}
		fd.write(format("},\n"));
	}
	else if(tatype == "array"){
		fd.write(format("[\n"));		
		foreach(k,v in ta){
			local vtype = type(v);
			if(vtype == "table" || vtype == "array"){
				doSaveTableArrayToFD(v, fd);
			}
			else
			{
				dumpValue(v);
			}
		}
		fd.write(format("],\n"));		
	}
	else throw "Only table/array suported";
}

function doSaveTableArrayToFileName(tbl, fname){
	local fd = file(fname, "w");
	fd.write("return [\n");
	doSaveTableArrayToFD(tbl, fd);
	fd.write("];");
	fd.close();
}

function doLoadTableArrayFromFileName(fname){
	local func = loadfile(fname);
	return func()[0];
}

function fillTemplate(template, data, nocache){
	data.escapeHtml <- escapeHtml;
	local mixFunc =getTemplate(template, nocache);
	mixFunc.call(data);
}

//
// Post
//
function split_filename(path){
  local result;
  path.gmatch("[/\\]?([^/\\]+)$", function(m){
	result = m;
	return false;
  });
  return result;
}

function insert_field (dest, key, value){
  local fld = dest.rawget(key, null);
  if (!fld) dest[key] <- value;
  else
  {
	if (type (fld) == "array") fld.push(value);
	else  dest[key] <- [fld, value];
  }
}

function multipart_data_get_field_names(headers, name_value){
  //foreach(k,v in headers) debug_print(k, "::", v, "\n");
  local disp_header = headers["content-disposition"] || "";
  local attrs = {};
  disp_header.gmatch(";%s*([^%s=]+)=\"(.-)\"", function(attr, val) {
	attrs[attr] <- val;
	//debug_print(attr, "::", val, "\n");
	return true;
  });
  name_value.push(attrs.name);
  name_value.push(attrs.rawget("filename", false) ? split_filename(attrs.filename) : null);
}

function multipart_data_break_headers(header_data){
	local headers = {};
	header_data.gmatch("([^%c%s:]+):%s+([^\n]+)", function(type, val){
		headers[type.tolower()] <- val;
		return true;
	});
	return headers;
}

function multipart_data_read_field_headers(input, state){
	local s, e, pos = state.pos;
	input.find_lua("\r\n\r\n", function(start, end){s=start; e=end; return false;}, pos, true);
	if( s ) {
		state.pos <- e;
		return multipart_data_break_headers(input.slice(pos, s));
	}
	else return null;
}

function multipart_data_read_field_contents(input, state){
	local boundaryline = "\r\n" + state.boundary;
	local s, e, pos = state.pos;
	input.find_lua(boundaryline, function(start, end){ s=start; e=end; return false;}, pos, true)
	if (s) {
		state.pos <- e;
		state.size <- s-pos;
		return input.slice(pos, s);
	}
	else {
		state.size <- 0;
		return null;
	}
}

function multipart_data_file_value(file_contents, file_name, file_size, headers){
  local value = { contents = file_contents, name = file_name, size = file_size };
  foreach( h, v in headers) {
	if (h != "content-disposition") value[h] <- v;
  }
  return value;
}

function multipart_data_parse_field(input, state){
	local headers, value;

	headers = multipart_data_read_field_headers(input, state);
	if (headers) {
		local name_value=[];
		multipart_data_get_field_names(headers, name_value);
		if (name_value[1]) { //file_name
			value = multipart_data_read_field_contents(input, state);
			value = multipart_data_file_value(value, name_value[1], state.size, headers);
			name_value[1] = value;
		}
		else name_value[1] = multipart_data_read_field_contents(input, state)
		return name_value;
	}
	return null;
}

function multipart_data_get_boundary(content_type){
	local boundary;
	content_type.gmatch("boundary%=(.-)$", function(m){
		boundary = m;
		return false;
	});
	return "--" + boundary;
}

function parse_multipart_data(input, input_type, tab=null){
	if(!tab) tab = {};
	local state = {};
	state.boundary <- multipart_data_get_boundary(input_type);
	input.find_lua(state.boundary, function(start, end){state.pos <- end+1;return false;}, 0, true);
	while(true){
		local name_value = multipart_data_parse_field(input, state);
		if(!name_value) break;
		insert_field(tab, name_value[0], name_value[1]);
	}
	return tab;
}

function parse_qs(qs, tab=null){
	if(!tab) tab = {};
	if (type(qs) == "string") {
		//debug_print(qs)
		qs.gmatch("([^&=]+)=([^&=]*)&?", function(key,val){
			//debug_print(key, "->", val)
			insert_field(tab, url_decode(key), url_decode(val));
			return true;
		});
	}
	else if (qs) throw("Request error: invalid query string");

	return tab;
}

function parse_qs_to_table(qs, tab=null){
	if(!tab) tab = {};
	if (type(qs) == "string") {
		//debug_print(qs)
		qs.gmatch("([^&=]+)=([^&=]*)&?", function(key,val){
			//debug_print(key, "->", val)
			key = url_decode(key);
			tab[key] <- url_decode(val);
			return true;
		});
	}
	else if (qs) throw("Request error: invalid query string");

	return tab;
}

function parse_qs_to_table_k(qs, tab=null, tabk=null){
	if(!tab) tab = {};
	if(!tabk) tabk = [];
	if (type(qs) == "string") {
		qs.gmatch("([^&=]+)=([^&=]*)&?", function(key, val){
		//debug_print(key, "->", val)
			key = url_decode(key);
			tabk.push(key);
			tab[key] <- url_decode(val);
			return true;
		});
	}
	else if (qs) throw("Request error: invalid query string");
	return tab;
}

function parse_post_data(input_type, data, tab = null){
	if(!tab) tab = {};
	local length = data.len();
	if (input_type.find("x-www-form-urlencoded") >= 0) parse_qs(data, tab);
	else if (input_type.find("multipart/form-data") >= 0) parse_multipart_data(data, input_type, tab);
	else if (input_type.find("SLE") >= 0) {
		local vv = [];
		sle2vecOfvec(data, vv);
		if (vv.len() > 0) {
			local names = vv[0];
			local values = vv[1];
			for (local i=0, len = names.len(); i < len; ++i){
				tab[names[i]] <- values[i];
			}
		}
	}
	return tab;
}

function get_post_fields(request, max_len=1024*1000){
	local data_len = (request.get_header("Content-Length") || "0").tointeger();
	local post_fields = {};
	if (data_len > 0 && data_len <= max_len) {
		local content_type = request.get_header("Content-Type") || "x-www-form-urlencoded";
		local data = request.read(data_len);
		
/*
		local fd = file(::APP_CODE_FOLDER + "/post.txt", "wb");
		fd.write(data, data.len());
		fd.close();
		debug_print(request.get_header("Content-Type"), "\n");
*/
		if(content_type == "application/json; charset=UTF-8"){
			return data;
		}
		parse_post_data(content_type, data, post_fields);
	}
	return post_fields;
}

local allowedUploadFileExtensions = {
	[".png"] = "image/png",
	[".jpg"] = "image/jpeg",
	[".gif"] = "image/gif",
}

function getMimeType(fname){
	local ext;
	fname.gmatch("(%.?[^%.\\/]*)$", @(m) ext=m);
	if( ext ) return allowedUploadFileExtensions.rawget(ext, "unknown");
	return "unknown";
}

function sanitizePath(path){
	//reorient separators
	path=path.gsub("\\", "/");

	//remove relativeness
	local relatpattern = "%.%.+";

	while (path.find_lua(relatpattern) > 0){
		path=path.gsub(relatpattern, "") //something like    /Repositories/swycartographer/res/msys/ + /mod
	}                              //gets converted to /Repositories/swycartographer/res/msys/mod

	//remove possible doubles
	relatpattern = "//";
	while(path.find(relatpattern) >= 0){
		path=path.gsub(relatpattern, "/");
	}

	//remove trailing slash
	if(path.endswith("/")) path=path.slice(0,-2);
	//remove slash at the begining
	if(path.startswith("/")) path = path.slice(1);
	path = path.gsub("[^A-Za-z0-9_%-%./]", "");
	return path
}

local allowedEditFileExtensions = {
	[".nut"] = true,
	[".tpl"] = true,
	[".html"] = true,
	[".css"] = true,
	[".js"] = true,
}

function isExtensionAllowed(fname){
	local ext;
	fname.gmatch("(%.?[^%.\\/]*)$", @(m) ext=m);
	if( ext ) return allowedEditFileExtensions.rawget(ext, false);
	return false;
}

function getFilesInPath(path, files=null, prefix=""){
	if(!files) files = [];
	foreach( file in sqfs.dir(path) ){
		if(file != "." && file != ".." ){
			local f = path + "/" + file;
			local pf
			if (prefix.len() > 0) pf = prefix + "/" + file;
			else pf = file;

			local attr = sqfs.attributes (f);

			if(attr.mode == "directory") getFilesInPath (f, files, pf);
			else
			{
				if( isExtensionAllowed(pf) ) files.push(pf);
				//foreach(name, value in attr) print (name, value);
			}
		}
	}
	files.sort();
	return files;
}

function getDbListFromStmt(stmt, maxSeconds=0){
	local error_code = 0;
	local rows = [];
	local db = stmt.get_db();
	if (maxSeconds){
		//local x = 0
		db.progress_handler(25, function(info) {
			//x = x +1
			//debug_print(x, "\n")
			//debug_print(x, ":", os.difftime(os.time(), info[0]), ":", info[1], "\n")
			if (os.difftime(os.time(), info[1]) > info[0]) return 1;
			return 0;
		}, [maxSeconds, os.time()]);
	}
	else db.progress_handler(null);

	try {
		while (stmt.next_row()){
			rows.push(stmt.asArray());
		}
	} catch(e){
		error_code = db.error_code();
	}

	if (maxSeconds) db.progress_handler(null);
	stmt.reset();
	//debug_print("\n", rows.len(), "\n");
	return [rows, error_code];
}

function strHasContent(v){
	if(v && v.len() > 0) return true;
	return false;
}

function send_http_error_500(request, err_msg){
	if(AT_DEV_DBG) {
		foreach(k,v in get_last_stackinfo()) debug_print("\n", k, ":", v);
		debug_print("\n", err_msg, "\n")
	}
	local response = format("HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain; charset=utf-8\r\nContent-Length: %d\r\n\r\n%s", 
		err_msg.len(),  err_msg);
	request.write(response, response.len());
	return true;
}

local uri_handlers = {
	["/SQ/testParams"] = function(request){
		request.print("HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n")
		request.print("<html><body><h1>Request Info</h1><ul>")
		foreach(k, v in request.info) {
			if ("table" == type(v) ){
				request.print(format("<li><b>%s</b>:</li><ul>", k));
				foreach( k2, v2 in v){
					request.print(format("<li><b>%s</b>: %s</li>", k2, v2));
				}
				request.print("</ul>");
			}
			else request.print(format("<li><b>%s</b>: %s</li>", k, (v == NULL ? "" : v).tostring()));
		}
		request.print("</ul></body></html>");
		return true;
	},
	["/SQ/logout"] = function(request){
		request.close_session();
		request.print(format("HTTP/1.1 302 Found\r\nLocation: http%s://%s\r\n\r\n", 
			request.info.is_ssl ? "s" : "", request.info.http_headers.Host))
		return true;
	},
	["/SQ/edit"] = function(request){
		//password protected
		bool_t canEdit = false;
		//print("EDIT_MD5_PASSWORD=", EDIT_MD5_PASSWORD, "\n")
		bool_t isViewOnly = globals.rawget("VIEW_MD5_PASSWORD", false) && request.check_password(VIEW_MD5_PASSWORD);
		if (!isViewOnly) canEdit = globals.rawget("EDIT_MD5_PASSWORD", false) && request.check_password(EDIT_MD5_PASSWORD);

		if(!(canEdit || isViewOnly) ) {
			request.send_authorization_request("r.dadbiz.es");
			return true;
		}

		table_t data = {
			file_name=null,
			content=null,
		};
		data.files <- getFilesInPath(APP_CODE_FOLDER);
		local query_string = request.info.query_string;
		bool_t isPost = request.info.request_method == "POST";
		if (isPost && canEdit) {
			local post_fields = get_post_fields(request);
			if (post_fields.rawget("save", false)) {
				local content = post_fields.rawget("content", null);
				if (content){
					data.file_name <- sanitizePath(post_fields.file_name);
					if (!isExtensionAllowed(data.file_name)) data.file_name = NULL;
					else
					{
						local fd = file(APP_CODE_FOLDER + "/" + data.file_name, "w");
						content = content.gsub("\r", "");
						fd.write(content);
						fd.close();
					}
				}
			}
		}
		else if( query_string ) data.file_name <- request.get_var(query_string, "file");

		//debug_print(data.search_id, "\n")
		if( data.file_name ){
			data.file_name = sanitizePath(data.file_name);
			if(! isExtensionAllowed(data.file_name) ) data.file_name = NULL;
			else
			{
				local fd = file(APP_CODE_FOLDER + "/" + data.file_name, "r");
				if( fd ){
					data.content <- fd.read(fd.len());
					fd.close();
				}
			}
		}

		//debug_tprint(data.company)

		local mFile = gmFile;
		mFile.clear(); //allways reset global vars
		data.mix_write <- function(str){ mFile.write(str || "")}
		fillTemplate("webappedit.tpl", data, AT_DEV_DBG);
		request.print(format("HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: %d\r\n\r\n", mFile.len()));
		request.write_blob(mFile);
		return true;
	},
}

function add_uri_hanlders(tbl){
	foreach(k,v in tbl) uri_handlers[k] <- v;
}

local uri_filters = [];

function add_uri_filters(func){
	uri_filters.push(func);
}

function remove_uri_filters(func){
	for(local i=0, len=uri_filters.len(); i < len; ++i){
		if(uri_filters[i] == func) {
			uri_filters.remove(i);
			break;
		}
	}
}

function apply_uri_filters(request){
	for(local i=0, len=uri_filters.len(); i < len; ++i){
		if(uri_filters[i](request)) {
			return true;
		}
	}
	return false;
}

if(AT_DEV_DBG || !globals.rawget("MyCompaniesUkLoaded", false)) {
	dofile(APP_CODE_FOLDER + "/companies-uk.nut");
}

if(AT_DEV_DBG || !globals.rawget("MyOurBizLoaded", false)) {
	dofile(APP_CODE_FOLDER + "/ourbiz.nut");
}

if(AT_DEV_DBG || !globals.rawget("MyOurShoppingCartLoaded", false)) {
	dofile(APP_CODE_FOLDER + "/ourbiz-shopping-cart.nut");
}

local ourbiz_password = md5("mingote:ourbiz.dadbiz.es:tr14pink");
function handle_request(request){
	//static content served by mongoose directly
	local request_uri = request.info.uri;
	//debug_print(request.get_option("document_root"), "::", request_uri, "\n")
	
	if(apply_uri_filters(request)) {
		return true;
	}
	
	if(request_uri.startswith("/DB/")){
		if(!request.check_password(ourbiz_password)) {
			request.send_authorization_request("ourbiz.dadbiz.es");
			return true;
		}
	}
	if (request_uri.endswith(".js") || request_uri.endswith(".css") ) return false;
	if (request_uri == "/index.html" || request_uri == "/" ) return uri_handlers["/search"](request);
	if( uri_handlers.rawget(request_uri, false) ) return uri_handlers[request_uri](request);
	return false;
}
