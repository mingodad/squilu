local socket_name = "/tmp/foo";
local client = socket.unix();
client.connect(socket_name);
//while (true) {
    local c = stdin.readn('c');
    client.send(c + " : Hello !\n");
//}
client.close();
