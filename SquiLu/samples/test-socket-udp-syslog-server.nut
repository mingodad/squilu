////////////////////////////////////////////////////////////////////////////-
// UDP sample: echo protocol client
// LuaSocket sample files
// Author: Diego Nehab
// RCS ID: $Id: echoclnt.lua,v 1.10 2005/01/02 22:44:00 diego Exp $
////////////////////////////////////////////////////////////////////////////-
local host = "127.0.0.1"
local port = 20007;

if (vargv.len() > 1){
    host = vargv[1] || host;
    port = vargv[2].tointeger() || port;
}
print("Binding to host '" + host + "' and port " + port + "...");
local udp = socket.udp();
udp.setsockname(host, port);
//udp.settimeout(5);
local sockname = udp.getsockname();
print("Waiting packets on " + sockname.address + ":" + sockname.port + "...");
while (true){
	local dgram = udp.receivefrom();
	if (dgram) {
		print(dgram.address + " : " + dgram.data);
		//udp.sendto(dgram.data, dgram.address, dgram.port);
	}
	else print(ip);
}
