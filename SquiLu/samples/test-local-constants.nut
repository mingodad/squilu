enum E {a=1.2, b=2.5};
//local fryday = "ok";
const fryday = 5;

local klass = class {};
local klass2 = class extends klass {};

function localF(){
	const fryday = 2;
	enum E {a, b};
	print(fryday, E.a, E.b);
	//fryday = 6;
	//print(fryday);
}

class localF3 {
	function localF(){}
};

local localF = "letter";

function localF2(){
	const fryday = 2;
	enum E {a, b};
	print(fryday, E.a, E.b);
	//fryday = 6;
	//print(fryday);
}


//print(fryday);
print(E.b);
localF();

/*
//local 
enum Enum {dad, red, blue};

const car = 3;

local function localFunc(){
	const mar = 999;
	print(mar, Enum.blue);
} 

localFunc();
*/