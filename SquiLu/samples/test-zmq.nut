local context = zmq_ctx();
print(__LINE__, context);
// Socket to talk to clients
local responder = context.socket (zmq_socket.REP);
print(__LINE__, responder);
print(__LINE__, responder.bind ("tcp://*:5555"));

while (1) {
	// Wait for next request from client
	local request = zmq_msg();
	print(__LINE__, request);
	//zmq_msg_init (&request);
	print(__LINE__, request.recv(responder, 0));
	print(format("Received Hello %s\n", request.data()));
	print(__LINE__, request.close ());

	// Do some 'work'
	os.sleep (1);

	// Send reply back to client
	local reply = zmq_msg(5);
	print(__LINE__, reply);
	//memcpy (zmq_msg_data (&reply), "World", 5);
	print(__LINE__, reply.data("World"));
	print(__LINE__, reply.send(responder, 0));
	print(__LINE__, reply.close ());
}
// We never get here but if we did, this would be how we end
print(__LINE__, responder.close());
//zmq_ctx_destroy (context);