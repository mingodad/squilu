//
// Copyright (c) 2007, Cameron Rich
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// * Neither the name of the axTLS project nor the names of its
//   contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

//
// Demonstrate the use of the axTLS library in Lua with a set of 
// command-line parameters similar to openssl. In fact, openssl clients 
// should be able to communicate with axTLS servers and visa-versa.
//
// This code has various bits enabled depending on the configuration. To enable
// the most interesting version, compile with the 'full mode' enabled.
//
// To see what options you have, run the following:
// > [lua] axssl s_server -?
// > [lua] axssl s_client -?
//
// The axtls/axtls shared libraries must be in the same directory or be found 
// by the OS.
//
//

local vargv_len = vargv.len();
// print version?
if (vargv_len == 1 && vargv[0] == "version"){
    print("axssl.nut " + axtls.ssl_version());
    os.exit(1);
}

//
// We've had some sort of command-line error. Print out the basic options.
//
function print_options(option){
    print("axssl: Error: '" + option + "' is an invalid command.");
    print("usage: axssl [s_server|s_client|version] [args ...]");
    os.exit(1);
}

//
// We've had some sort of command-line error. Print out the server options.
//
function print_server_options(build_mode, option){
    local cert_size = axtls.get_config(axtls.SSL_MAX_CERT_CFG_OFFSET);
    local ca_cert_size = axtls.get_config(
            axtls.SSL_MAX_CA_CERT_CFG_OFFSET);

    print("unknown option " + option)
    print("usage: s_server [args ...]");
    print(" -accept\t- port to accept on (default is 4433)");
    print(" -quiet\t\t- No server output");

    if (build_mode >= axtls.SSL_BUILD_SERVER_ONLY) {
        print(" -cert arg\t- certificate file to add (in addition to default) to chain -");
        print("\t\t  Can repeat up to " + cert_size + " times");
        print(" -key arg\t- Private key file to use - default DER format");
        print(" -pass\t\t- private key file pass phrase source");
    }

    if (build_mode >= axtls.SSL_BUILD_ENABLE_VERIFICATION){
        print(" -verify\t- turn on peer certificate verification");
        print(" -CAfile arg\t- Certificate authority - default DER format");
        print("\t\t  Can repeat up to " + ca_cert_size + " times");
    }

    if (build_mode == axtls.SSL_BUILD_FULL_MODE) {
        print(" -debug\t\t- Print more output");
        print(" -state\t\t- Show state messages");
        print(" -show-rsa\t- Show RSA state");
    }

    os.exit(1);
}

//
// We've had some sort of command-line error. Print out the client options.
//
function print_client_options(build_mode, option){
    local cert_size = axtls.get_config(axtls.SSL_MAX_CERT_CFG_OFFSET);
    local ca_cert_size = axtls.get_config(
            axtls.SSL_MAX_CA_CERT_CFG_OFFSET);

    print("unknown option " + option);

    if (build_mode >= axtls.SSL_BUILD_ENABLE_CLIENT ){
        print("usage: s_client [args ...]");
        print(" -connect host:port - who to connect to (default is localhost:4433)");
        print(" -verify\t- turn on peer certificate verification");
        print(" -cert arg\t- certificate file to use - default DER format");
        print(" -key arg\t- Private key file to use - default DER format");
        print("\t\t  Can repeat up to " + cert_size + " times");
        print(" -CAfile arg\t- Certificate authority - default DER format");
        print("\t\t  Can repeat up to " + ca_cert_size + "times");
        print(" -quiet\t\t- No client output");
        print(" -pass\t\t- private key file pass phrase source");
        print(" -reconnect\t- Drop and re-make the connection with the same Session-ID");

        if (build_mode == axtls.SSL_BUILD_FULL_MODE){
            print(" -debug\t\t- Print more output");
            print(" -state\t\t- Show state messages");
            print(" -show-rsa\t- Show RSA state");
        }
    }
    else print("Change configuration to allow this feature");

    os.exit(1);
}

// Implement the SSL server logic. 
function do_server(build_mode){
    local i = 1;
    local v;
    local port = 4433;
    local options = axtls.SSL_DISPLAY_CERTS;
    local quiet = false;
    local password = "";
    local private_key_file = null;
    local cert_size = axtls.get_config(axtls.SSL_MAX_CERT_CFG_OFFSET);
    local ca_cert_size = axtls.get_config(axtls.SSL_MAX_CA_CERT_CFG_OFFSET);
    local cert = [];
    local ca_cert = [];

    while (i < vargv_len) {
        if (vargv[i] ==  "-accept"){
            if (i >= (vargv_len-1)){
                print_server_options(build_mode, arg[i]);
            }

            ++i;
            port = vargv[i];
	}
        else if (vargv[i] == "-quiet"){
            quiet = true;
            options = options & (~axtls.SSL_DISPLAY_CERTS);
	}
        else if (build_mode >= axtls.SSL_BUILD_SERVER_ONLY){
            if (vargv[i] == "-cert"){
                if (i >= (vargv_len-1) || cert.len() >= cert_size){
                    print_server_options(build_mode, vargv[i]); 
                }

                ++i;
                cert.push(vargv[i]);
	    }
            else if (vargv[i] == "-key"){
                if (i >= (vargv_len-1)){
                    print_server_options(build_mode, vargv[i]); 
                }

                ++i;
                private_key_file = vargv[i];
                options = options | axtls.SSL_NO_DEFAULT_KEY;
	    }
            else if (vargv[i] == "-pass"){
                if (i >= (vargv_len-1)){
                    print_server_options(build_mode, vargv[i]); 
                }

                ++i;
                password = vargv[i];
	    }
            else if (build_mode >= axtls.SSL_BUILD_ENABLE_VERIFICATION){
                if (vargv[i] == "-verify"){
                    options = options | axtls.SSL_CLIENT_AUTHENTICATION;
		}
                else if (vargv[i] == "-CAfile"){
                    if (i >= (vargv_len-1) || ca_cert.len() >= ca_cert_size){
                        print_server_options(build_mode, vargv[i]);  
                    }

                    ++i;
                    ca_cert.push(vargv[i]);
		}
                else if (build_mode == axtls.SSL_BUILD_FULL_MODE){
                    if (vargv[i] == "-debug"){
                        options = options | axtls.SSL_DISPLAY_BYTES;
		    }
                    else if (vargv[i] == "-state") options = options | axtls.SSL_DISPLAY_STATES;
                    else if (vargv[i] == "-show-rsa") options = options | axtls.SSL_DISPLAY_RSA;
                    else print_server_options(build_mode, vargv[i]);
		}
                else print_server_options(build_mode, vargv[i]);
	    }
            else print_server_options(build_mode, vargv[i]);
	}
        else print_server_options(build_mode, vargv[i]);

        ++i;
    }

    // Create socket for incoming connections
    local server_sock = socket.tcp();
    //server_sock.bind("*", port);
    server_sock.setoption("reuseaddr", true);
    server_sock.bind("*", port);
    server_sock.listen();

    //////////////////////////////////////////////////////////////////////////-
    // This is where the interesting stuff happens. Up until now we've
    // just been setting up sockets etc. Now we do the SSL handshake.
    //////////////////////////////////////////////////////////////////////////-
    local ssl_ctx = axtls.ssl_ctx(options, axtls.SSL_DEFAULT_SVR_SESS);
    if (ssl_ctx == null) throw("Error: Server context is invalid");

    if (private_key_file != null){
        local obj_type = axtls.SSL_OBJ_RSA_KEY;

        if (private_key_file.find(".p8") >= 0 ) obj_type = axtls.SSL_OBJ_PKCS8;
        else if (private_key_file.find(".p12") >= 0) obj_type = axtls.SSL_OBJ_PKCS12;

        if (ssl_ctx.ssl_obj_load(obj_type, private_key_file, 
                                        password) != axtls.SSL_OK)
		throw("Private key '" + private_key_file + "' is undefined.");
    }

    foreach(v in cert) {
        if (ssl_ctx.ssl_obj_load(axtls.SSL_OBJ_X509_CERT, v, "") != 
                                        axtls.SSL_OK)
		throw("Certificate '" + v + "' is undefined.");
    }

    foreach(v in ca_cert){
        if (ssl_ctx.ssl_obj_load(axtls.SSL_OBJ_X509_CACERT, v, "") != 
                                        axtls.SSL_OK)
		throwr("Certificate '" + v + "' is undefined.");
    }

    while (true){
        if (!quiet) print("ACCEPT");
        local client_sock = server_sock.accept();
        local ssl = ssl_ctx.server_new(client_sock.getfd());

        // do the actual SSL handshake
        local connected = false;
        local res;

        while (true){
            socket.select([client_sock], []);
	    result = ssl.read();
	    if(type(result) == "string"){
                foreach(v in buf) print1(v);
	    }
	    else
	    {
		if (res == axtls.SSL_OK){ // connection established and ok
			if (ssl.handshake_status() == axtls.SSL_OK){
			    if (!quiet && !connected){
				display_session_id(ssl);
				display_cipher(ssl);
			    }
			    connected = true;
			}
		}
		else if (res < axtls.SSL_OK){ 
			if (!quiet) axtls.display_error(res);
			break;
		}
	    }
        }

        // client was disconnected or the handshake failed.
        print("CONNECTION CLOSED");
        ssl.free();
        client_sock.close();
    }

    ssl_ctx.free();
}

//
// Implement the SSL client logic.
//
function do_client(build_mode){
    local i = 1;
    local v;
    local port = 4433;
    local options = axtls.SSL_SERVER_VERIFY_LATER | axtls.SSL_DISPLAY_CERTS;
    local private_key_file = null;
    local reconnect = 0;
    local quiet = false;
    local password = "";
    local session_id = [];
    local host = "127.0.0.1";
    local cert_size = axtls.get_config(axtls.SSL_MAX_CERT_CFG_OFFSET);
    local ca_cert_size = axtls.get_config(axtls.SSL_MAX_CA_CERT_CFG_OFFSET);
    local cert = [];
    local ca_cert = [];

    while (i < vargv_len){
        if (vargv[i] == "-connect"){
            if (i >= (vargv_len-1))  print_client_options(build_mode, vargv[i]);

            ++i;
	    local str = vargv[i];
            local t = str.find(":");
            host = str.substr(0, t);
            port = str.substr(t+1);
	}
        else if (vargv[i] == "-cert"){
            if (i >= (vargv_len-1) || cert.len() >= cert_size) print_client_options(build_mode, vargv[i]) ;

            ++i;
            cert.push(vargv[i]);
	}
        else if (vargv[i] == "-key"){
            if (i >= (vargv_len-1)) print_client_options(build_mode, vargv[i]);

            ++i;
            private_key_file = vargv[i];
            options = options | axtls.SSL_NO_DEFAULT_KEY;
	}
        else if (vargv[i] == "-CAfile"){
            if (i >= vargv_len || ca_cert.len() >= ca_cert_size) print_client_options(build_mode, vargv[i]) ;

            ++i;
            ca_cert.push(vargv[i]);
	}
        else if (vargv[i] == "-verify") options = options & (~axtls.SSL_SERVER_VERIFY_LATER);
        else if (vargv[i] == "-reconnect") reconnect = 4;
        else if (vargv[i] == "-quiet") {
		quiet = true;
		options = options & (~axtls.SSL_DISPLAY_CERTS);
	}
        else if (vargv[i] == "-pass") {
            if (i >= (vargv_len-1)) print_server_options(build_mode, vargv[i]);

            ++i;
            password = vargv[i];
	}
        else if (build_mode == axtls.SSL_BUILD_FULL_MODE){
            if (vargv[i] == "-debug") options = options | axtls.SSL_DISPLAY_BYTES;
            else if (vargv[i] == "-state") options = options  | axtls.SSL_DISPLAY_STATES;
            else if (vargv[i] == "-show-rsa") options = options | axtls.SSL_DISPLAY_RSA;
            else    // don't know what this is
                print_client_options(build_mode, vargv[i]);
	}
        else    // don't know what this is
            print_client_options(build_mode, vargv[i]);

        ++i;
    }

    local client_sock = socket.tcp();
    client_sock.connect(host, port);
    local ssl;
    local res;

    if (!quiet) print("CONNECTED");

    //////////////////////////////////////////////////////////////////////////-
    // This is where the interesting stuff happens. Up until now we've
    // just been setting up sockets etc. Now we do the SSL handshake.
    //////////////////////////////////////////////////////////////////////////-
    local ssl_ctx = axtls.ssl_ctx(options, axtls.SSL_DEFAULT_CLNT_SESS);

    if (ssl_ctx == null) throw("Error: Client context is invalid");

    if (private_key_file != null){
        local obj_type = axtls.SSL_OBJ_RSA_KEY;

        if (private_key_file.endswith(".p8")) obj_type = axtls.SSL_OBJ_PKCS8;
        else if (private_key_file.endswith(".p12")) obj_type = axtls.SSL_OBJ_PKCS12 ;

        if (ssl_ctx.obj_load(obj_type, private_key_file, 
                                        password) != axtls.SSL_OK)
		throw("Private key '" + private_key_file + "' is undefined.");
    }

    foreach(v in cert) {
        if (ssl_ctx.obj_load(axtls.SSL_OBJ_X509_CERT, v, "") != 
                                        axtls.SSL_OK)
		throw("Certificate '" + v + "' is undefined.");
    }

    foreach(v in ca_cert) {
        if (ssl_ctx.obj_load(axtls.SSL_OBJ_X509_CACERT, v, "") != 
                                        axtls.SSL_OK)
		throwr("Certificate '" + v + "' is undefined.");
    }

    // Try session resumption?
    if (reconnect != 0){
        session_id = null;
        local sess_id_size = 0;

        while (reconnect > 0){
            reconnect = reconnect - 1;
            ssl = ssl_ctx.client_new(client_sock.getfd(), session_id, sess_id_size);

            res = ssl.handshake_status();
            if (res != axtls.SSL_OK){
                if (!quiet) axtls.display_error(res);
                ssl.free();
                os.exit(1);
            }

            display_session_id(ssl);
            session_id = ssl.get_session_id();
            sess_id_size = ssl.get_session_id_size();

            if (reconnect > 0) {
                ssl.free();
                client_sock.close();
		client_sock = socket.tcp();
                client_sock.connect(host, port);
            }

        }
    }
    else ssl = ssl_ctx.client_new(client_sock.getfd());

    // check the return status
    res = ssl.handshake_status();
    if (res != axtls.SSL_OK){
        if (!quiet) axtls.display_error(res);
        os.exit(1);
    }

    if (!quiet){
        local common_name = ssl.get_cert_dn(axtls.SSL_X509_CERT_COMMON_NAME);

        if (common_name != null) print("Common Name:\t\t\t" + common_name);

        display_session_id(ssl);
        display_cipher(ssl);
    }

    while (true){
        local line = stdin.read_line();
	if (line == null) break;
	
	line += "\r\x00";

        res = ssl.write(line, line.len());
        if (res < axtls.SSL_OK){
            if (!quiet) axtls.display_error(res);
            break;
        }
    }

    ssl_ctx.free();
    client_sock.close();
}

//
// Display what cipher we are using 
//
function display_cipher(ssl){
    print1("CIPHER is ");
    local cipher_id = ssl.get_cipher_id();

    switch(cipher_id){
	case axtls.SSL_AES128_SHA:  print("AES128-SHA"); break;
	case axtls.SSL_AES256_SHA:  print("AES256-SHA"); break;
	case axtls.SSL_RC4_128_SHA:  print("RC4-SHA"); break;
	case axtls.SSL_RC4_128_MD5:  print("RC4-MD5"); break;
	default:
		print("Unknown - " + cipher_id);
    }
}

//
// Display what session id we have.
//
function display_session_id(ssl){
    local session_id = ssl.get_session_id();
    local v;

    if (session_id.len() > 0){
        print("////-BEGIN SSL SESSION PARAMETERS////-");
        foreach(v in session_id) print1(format("%02x", v));
        print("\n////-END SSL SESSION PARAMETERS////-");
    }
}

//
// Main entry point. Doesn't do much except works out whether we are a client
// or a server.
//
if (vargv_len == 0 || (vargv[0] != "s_server" && vargv[0] != "s_client")){
    print_options(vargv_len > 0 && vargv[0] || "");
}

local build_mode = axtls.get_config(axtls.SSL_BUILD_MODE);
local xx = vargv[0] == "s_server" && do_server(build_mode) || do_client(build_mode);
os.exit(0);

