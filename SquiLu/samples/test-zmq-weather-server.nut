//
// Weather update server
// Binds PUB socket to tcp://*:5556
// Publishes random weather updates
//
// Author: Robert G. Jakabosky <bobby@sharedrealm.com>
//


// Prepare our context and publisher
local context = zmq_ctx();
print(__LINE__, context);
local publisher = context.socket(zmq_socket.PUB);
print(__LINE__, publisher);
print(__LINE__, publisher.bind("tcp://*:5556"));
print(__LINE__, publisher.bind("ipc://weather.ipc"));

// Initialize random number generator
math.srand(os.time());
while (1) {
	// Get values that will fool the boss
	local zipcode, temperature, relhumidity;
	zipcode = math.random(0, 99999);
	//zipcode = 10001;
	temperature = math.random(-80, 135);
	relhumidity = math.random(10, 60);

	// Send message to all subscribers
	//print(__LINE__, 
	publisher.send(format("%05d %d %d", zipcode, temperature, relhumidity)); //);
	//os.sleep(0.0001);
}
publisher.close();
context.destroy();
