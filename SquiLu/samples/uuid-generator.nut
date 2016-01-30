function pseudoUuid(){
	return os.time() + "-" + (math.random()*1000000).tointeger() + "-" + (math.random()*1000000).tointeger();
}

for(var i=0; i < 10; ++i) print(os.getuuid());

for(var i=0; i < 10; ++i) print(pseudoUuid());