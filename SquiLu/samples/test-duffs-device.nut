local tbl = {len2 = function() {return "dad";}, }

print(table_size(tbl), table_len(tbl), tbl.len2());

local s = "dad";

switch(s){
	case "dad": print(1, s); break;
	case "tais": print(2, s); break;
	default:
		print(3, s);
}

local function process(i){
	return i+i;
}

local count = 10000;

local values = array(1000);
for(var i=0, len=values.len(); i < len; ++i) values[i] = i; 

local start = os.clock();
for(var k=0; k < count; ++k){
	var iterations = math.floor(values.len() / 8);
	var startAt = values.len() % 8;
	var i = 0;

	do {
	    switch(startAt){
		case 0: process(values[i++]);
		case 7: process(values[i++]);
		case 6: process(values[i++]);
		case 5: process(values[i++]);
		case 4: process(values[i++]);
		case 3: process(values[i++]);
		case 2: process(values[i++]);
		case 1: process(values[i++]);
	    }
	    startAt = 0;
	} while (--iterations > 0);
}
print("Duff's Device with switch took:", os.clock()-start);

start = os.clock();
for(var k=0; k < count; ++k){
	var iterations = math.floor(values.len() / 8);
	var leftover = values.len() % 8;
	var i = 0;

	if (leftover > 0){
	    do {
		process(values[i++]);
	    } while (--leftover > 0);
	}

	do {
	    process(values[i++]);
	    process(values[i++]);
	    process(values[i++]);
	    process(values[i++]);
	    process(values[i++]);
	    process(values[i++]);
	    process(values[i++]);
	    process(values[i++]);
	} while (--iterations > 0);
}
print("Duff's Device without switch took:", os.clock()-start);

start = os.clock();
for(var k=0; k < count; ++k){
	for(var i=0, len=values.len(); i < len; ++i) process(values[i]);
}
print("for loop took:", os.clock()-start);

