//
// Weather update client
// Connects SUB socket to tcp://localhost:5556
// Collects weather updates and finds avg temp in zipcode
//
// Author: Robert G. Jakabosky <bobby@sharedrealm.com>
//

local context = zmq_ctx();
print(__LINE__, context);
// Socket to talk to server
print("Collecting updates from weather server...");
local subscriber = context.socket(zmq_socket.SUB);
print(__LINE__, subscriber);
print(__LINE__, subscriber.connect( vargv.len() > 1 ? vargv[1] : "tcp://localhost:5556"));

// Subscribe to zipcode, default is NYC, 10001
local filter = vargv.len() > 0 ? vargv[0] : "10001 ";
print(__LINE__, subscriber.setsockopt(zmq_socket.SUBSCRIBE, filter));

// Process 100 updates
local update_nbr = 0;
local total_temp = 0;
for(local n=0; n < 100; ++n){
	local message = subscriber.recv();
	print(message);
	message.gmatch("([%d-]*) ([%d-]*) ([%d-]*)", function(zipcode, temperature, relhumidity){
			total_temp += temperature.tointeger();
			++update_nbr;			
		});
}
print(format("Average temperature for zipcode '%s' was %dF, total = %d", filter, (total_temp / update_nbr), total_temp));

subscriber.close();
context.destroy();
