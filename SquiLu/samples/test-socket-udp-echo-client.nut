////////////////////////////////////////////////////////////////////////////-
// UDP sample: echo protocol client
// LuaSocket sample files
// Author: Diego Nehab
// RCS ID: $Id: echoclnt.lua,v 1.10 2005/01/02 22:44:00 diego Exp $
////////////////////////////////////////////////////////////////////////////-
local host = vargv.len() > 1 ? vargv[1] : "localhost";
local port = vargv.len() > 2 ? vargv[2] : 20007;
host = socket.toip(host);
print(host.address)
foreach(k,v in host.resolved) {
	print(k,v);
	if(type(v) == "array"){
		foreach(ak,av in v) print(ak, av);
	}
}
local udp = socket.udp();

udp.setpeername(host.address, port);
print("Using remote host '" + host + "' and port " + port + "...");
while (true){
	//local line = stdin.read(4);
	//if(!line || line == "") os.exit();
	local line = "dad udp !";
	udp.send(line);
	local dgram = udp.receive();
	print(dgram);
}