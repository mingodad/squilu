// create a TCP socket and bind it to the local host, at any port
local server = socket.tcp();
//server.bind("*", 0);
server.setoption("reuseaddr", true);
server.bind("*", 0);
server.listen();
// find out which port the OS chose for us
local tbl = server.getsockname();
local ip = tbl.address, port = tbl.port ;
// print a message informing what's up
print(ip, port);
print("Please telnet to localhost on port " + port)
print("After connecting, you have 10s to enter a line to be echoed")
// loop forever waiting for clients
while (1){
  // wait for a connection from any client
  local client = server.accept();
  // make sure we don't block waiting for this client's line
  client.settimeout(10);
  try {
	// receive the line
	local line = client.receive("*l");
	// if there was no error, send it back to the client
	print(line);
	client.send(line + "\n");
  }
   catch(e){
	print(e);
  }
  // done with client, close the object
  client.close();
}