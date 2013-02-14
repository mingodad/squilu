function localF(){
	const fryday = 5;
	enum E {a, b};
	print(fryday, E.a, E.b);
	//fryday = 6;
	//print(fryday);
}
//print(fryday);
localF();
enum E {a=1.2, b=2.5};
print(E.b);

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