local A = class {
	a = null;
	b = null;
	static c = 99;
	static d = [];
}

local a = A();
local b = A();

a.a = 3;
b.a = 5;
b.d.push(44);
a.d.push(55);
a.c = 66;
b.c = 77;

print(a.a, a.c, b.a, b.c, a.d[0], b.d[1]);

local B = class {
	static cfields = ["a", "b", "c", "d"];
	constructor(){
		foreach(k in cfields) this[k] = 0;
	}
}

foreach(k in B.cfields) B[k] <- null;

local cb = B();

foreach(k,v in B) print(k, cb[k]);

local C = class extends B {
	cc = null;
}

local cc = C();
cc.cc = 43;
print(cc.cc);