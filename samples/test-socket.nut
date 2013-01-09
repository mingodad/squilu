////////////////////////////////////////////////////////////////////////////-
// UDP sample: echo protocol client
// LuaSocket sample files
// Author: Diego Nehab
// RCS ID: $Id: echoclnt.lua,v 1.10 2005/01/02 22:44:00 diego Exp $
////////////////////////////////////////////////////////////////////////////-
local host = "localhost"
local port = 7
if (vargv.len() > 0){
    host = vargv[0] || host;
    port = vargv[1] || port;
}
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