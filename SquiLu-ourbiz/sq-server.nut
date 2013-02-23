local globals = getroottable();
if(!globals.get("APP_CODE_FOLDER", false)) ::APP_CODE_FOLDER <- ".";

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

	if (globals.get("VIEW_MD5_PASSWORD", false)){
		extra_code += format("VIEW_MD5_PASSWORD <- \"%s\";\n", VIEW_MD5_PASSWORD);
	} else extra_code += "VIEW_MD5_PASSWORD <- nul;\n";

	if (globals.get("EDIT_MD5_PASSWORD", false)){
		extra_code += format("EDIT_MD5_PASSWORD <- \"%s\";\n", EDIT_MD5_PASSWORD);
	} else extra_code += "EDIT_MD5_PASSWORD <- null;\n";

	if (globals.get("AT_DEV_DBG", false)){
		extra_code += "AT_DEV_DBG <- true;\n"
	} else extra_code += "AT_DEV_DBG <- false;\n"

	return compilestring(format("%s\n%s", extra_code, code));
}

local mongoose_start_params = {
	error_log_file = "sq-mongoose.log",
	listening_ports = "8080",
	document_root = "./s",
	//num_threads = 5,

	//cgi_extensions = "lua",
	//cgi_interpreter = "/usr/bin/lua",
	//cgi_interpreter = "C:\\Lua\\5.1\\lua.exe",
	ssl_certificate = "axTLS.x509_512.pem",
        //"ssl_certificate", "axTLS.x509_1024.pem",
        ssl_chain_file = "axTLS_x509_512.cer",
	extra_mime_types = ".xsl=application/xml",
	master_plugin = function(){
		debug_print("done master_plugin\n");
	},
	master_plugin_exit = function(){
		debug_print("done master_plugin_exit\n");
	},
	//functions to be used by each independent lua vm
	user_callback_setup = getUserCallbackSetup("sq-server-plugin.nut"),
	user_callback_exit = function(){
		debug_print("done user_callback_exit\n");
	},
	user_callback = function(event, request){
		if(event == "MG_NEW_REQUEST"){
			//debug_print("\n", request.get_option("num_threads"), request.get_conn_buf());
			if(AT_DEV_DBG || !this.get("handle_request", false)) {
				dofile(APP_CODE_FOLDER + "/sq-server-plugin.nut");
			}
			try {
				return handle_request(request);
			}
			catch(exep){
				if(AT_DEV_DBG) {
					foreach(k,v in get_last_stackinfo()) debug_print("\n", k, ":", v);
					debug_print("\n", exep, "\n")
				}
				local response = format("HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain; charset=utf-8\r\nContent-Length: %d\r\n\r\n%s", 
					exep.len(),  exep);
				request.write(response, response.len());
				return true;
			}
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

