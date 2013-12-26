local socket_name = "/tmp/foo";
local server = socket.unix();
try {
	os.remove(socket_name);   /* in case it already exists */
} catch(e) {}
server.bind(socket_name);

print(server);

server.listen();
local c = server.accept();
print(c);
//while(true) {
  try {
	local resp = c.receive();
	print(resp[0], resp[1]);
  }
   catch(e){
	print(e);
	//break;
  }
//}
c.close();
server.close();
os.remove(socket_name);
