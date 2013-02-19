class A {
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

print(a.a, b.a, b.c, a.d[0], b.d[1]);