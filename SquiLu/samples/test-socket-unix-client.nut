local client = socket.unix();
client.connect("/tmp/foo");
//while (true) {
    local c = stdin.readn('c');
    client.send(c + " : Hello !\n");
//}
client.close();