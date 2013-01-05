function download(host, file, extra_header=null){
	local sock = socket.tcp();
	sock.connect(host, 80);
	local info = sock.getpeername()
	print(info.address, info.port);
	//sock.settimeout(1, "t");

	local count = 0;    // counts number of bytes read
	local req = "GET " + file + " HTTP/1.1\r\nHost: " + host + "\r\n";
	if (extra_header) req += extra_header + "\r\n";
	req += "\r\n";
	print(req, req.len());
	sock.send(req);
	local s
	local tbl = {};
	local len = 0;
	while (true){
		s = sock.receive("*l");
		print("s", s);
		//if err == "closed" then break end
		if (s.len() == 0) break;
		local slen;
		s.gmatch("Content%-Length: (%d+)", function(m){ slen=m; return false;});
		if (slen) {
			len = slen.tointeger();
		}
	}
	s = sock.receive(len);
	sock.close();
	//print(file, count)
	return s;
}

local page = download("w3.impa.br", "/~diego/software/luasocket/tcp.html");
//local page = download("www.google.co.uk", "/");
print(page);