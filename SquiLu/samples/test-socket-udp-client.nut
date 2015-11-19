local host = "*";
local port = vargv.len() > 1 ? vargv[1] : 20007;
local udp = socket.udp();
udp.setsockname(host, port);
udp.settimeout(-1);
print("Using remote host '" + host + "' and port " + port + "...");
while (true){
	//local line = stdin.read(4);
	//if(!line || line == "") os.exit();
	//local line = "dad udp !";
	//udp.send(line);
	local dgram = udp.receive();
	print(dgram.len(), dgram);
	//socket.sleep(0.01);
}