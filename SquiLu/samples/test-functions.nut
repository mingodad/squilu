class A {
	name = null;
	function print(str){
		::print(name, str);
	}
}

local a = A();
a.name = "dad";
a.print("red");

local b = A();
b.name = "tais";
b.print("blue");

local af = A.print;

af.call(a, "white");
af.call(b, "cyan");


