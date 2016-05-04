/*
 * Copyright (C) 2013 by Domingo Alvarez Duarte <mingodad@gmail.com>
 *
 * Licensed under GPLv3, see http://www.gnu.org/licenses/gpl.html.
 */
 
local globals = getroottable();
if(!table_rawget(globals, "APP_CODE_FOLDER", false)) ::APP_CODE_FOLDER <- ".";

WIN32 <- os.getenv("WINDIR") != null;

/*
local _old_import = import;
::__imported_files <-  {};
::import = function(fn){
}
*/

AT_DEV_DBG <- true;

function getUserCallbackSetup(fn){
	local fd = file(APP_CODE_FOLDER + "/" + fn, "r");
	local code = fd.read(fd.len());
	fd.close();
	local extra_code = format("APP_CODE_FOLDER <- \"%s\";\n", APP_CODE_FOLDER);
	
	local checkGlobal = function(gv)
	{
		if (table_rawin(globals, gv)){
			extra_code += format(gv + " <- \"%s\";\n", table_rawget(globals, gv));
		} else extra_code += gv + " <- false;\n";
	}

	checkGlobal("GLOBAL_PASSWORD_DOMAIN");
	checkGlobal("GLOBAL_MD5_PASSWORD");
	checkGlobal("VIEW_MD5_PASSWORD");
	checkGlobal("EDIT_MD5_PASSWORD");
	checkGlobal("EDIT_MD5_PASSWORD");

	if (table_rawget(globals, "AT_DEV_DBG", false)){
		extra_code += "AT_DEV_DBG <- true;\n";
	} else extra_code += "AT_DEV_DBG <- false;\n";
	
	code = extra_code + "\n" + code;
	if(table_rawin(globals, "addExtraCodeToUserCallbackSetup"))
	{
		code = addExtraCodeToUserCallbackSetup() + code;
	}

	return compilestring( code );
}

local mongoose_start_params = {
	error_log_file = "sq-mongoose.log",
	listening_ports = "8080",
	document_root = "./s",
	//num_threads = 50,
	//enable_keep_alive = "yes",
	//enable_tcp_nodelay = "yes",
	//request_timeout_ms = "30000",

	//cgi_extensions = "lua",
	//cgi_interpreter = "/usr/bin/lua",
	//cgi_interpreter = "C:\\Lua\\5.1\\lua.exe",
	ssl_certificate = "axTLS.x509_512.pem",
        //"ssl_certificate", "axTLS.x509_1024.pem",
        ssl_chain_file = "axTLS_x509_512.cer",
	extra_mime_types = ".xsl=application/xml,.json=application/json; charset=utf-8",
	master_plugin = function(){
		//companies_uk_db_mem_global <- SQLite3("file:companies_uk_db?mode=memory&cache=shared");
		//companies_uk_db_mem_global.restore("./companies-uk/companies-uk-2014-07.db");
		//ourbiz_db_mem_global <- SQLite3("file:ourbiz_db?mode=memory&cache=shared");
		//ourbiz_db_mem_global.restore("/home/mingo/dev/FrontAccountLua/ourbiz.db");

		debug_print("done master_plugin\n");
	},
	master_plugin_exit = function(){
		//companies_uk_db_mem_global.close();
		//ourbiz_db_mem_global.close();
		debug_print("done master_plugin_exit\n");
	},
	//functions to be used by each independent lua vm
	user_callback_setup = getUserCallbackSetup("sq-server-plugin.nut"),
	user_callback_exit = function(){
		debug_print("done user_callback_exit\n");
	},
	user_callback = function(event, request){
		//debug_print("\nevent :\n", event);
		if(event == "MG_NEW_REQUEST"){
			if(GLOBAL_MD5_PASSWORD)
			{
				if(request.info.uri == "/SQ/logout")
				{
					request.close_session();
					request.print(format("HTTP/1.1 302 Found\r\nLocation: http%s://%s\r\n\r\n", 
						request.info.is_ssl ? "s" : "", request.info.http_headers.Host));
					return true;
				}
/*
				if(!request.check_password(GLOBAL_MD5_PASSWORD)) {
					request.send_authorization_request(GLOBAL_PASSWORD_DOMAIN);
					return true;
				}*/
			}
			//debug_print("\n", request.get_option("num_threads"), request.get_conn_buf());
			if(AT_DEV_DBG || !table_get(this, "handle_request", false)) {
				 //when developing we reload everything on each request
				dofile(APP_CODE_FOLDER + "/sq-server-plugin.nut");
			}
			local result;
			try {
				//debug_print("\nHttp :\n", request.info.uri);
				result = handle_request(request);
			}
			catch(exep){
				result = send_http_error_500(request, exep);
			}
			if(AT_DEV_DBG && table_get(this, "onDevCleanup", false)) {
				 //when developing if we need to cleanup something here is the place
				onDevCleanup();
			}
			return result;
		}
		else if(event == "MG_EVENT_LOG"){
			//debug_print("\n", request.info.log_message);
			//return true;
			return false;
		}
		else if(event == "MG_HTTP_ERROR"
			|| event == "MG_INIT_SSL"){
				return false;
		}
	},
}

local mg;

function appServerStart(port, document_root){
	mg = Mongoose();
	//mongoose_start_params.num_threads <- "2";
	mongoose_start_params.listening_ports = port;
	mongoose_start_params.document_root = document_root;
	mg.show_errors_on_stdout(true);
	mg.start(mongoose_start_params);
}

//appServerStart(8087, "./s");
//stdin.readn('c');

function appServerStop(){
	if(mg) {
		mg.stop();
		mg = null;
	}
}

//print( lfs.currentdir())

//
// myio = io.open("luadump.txt", "wb")
// myio:write(string.dump(start_params.user_callback))
// myio:close()

// run until the user presses enter
//io.read()

//appServerStop()

