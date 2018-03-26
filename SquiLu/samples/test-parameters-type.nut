let isDone2: bool_t = false;
let isDone: boolean = false;
int_t ifunc(int_t p);
extern string_t sfunc(string_t p);

array_t ary = [1,2,3];

int_t vi;
float_t vf;
string_t vs;


function one(a: integer, b : integer, c : number) : number
{
	return a + b + c;
}

int_t two(int_t a, int_t b, int_t c)
{
	return a + b + c;
}

string_t three(string_t a, string_t b, string_t c)
{
	return a + b + c;
}

print(one(1111,2222,3333));
print(two(1111,2222,3333));

int_t vi0;
int_t vi1 = 111;
int_t vi2 = 222;
int_t vi3 = 333;

print(two(vi1,vi2,vi3));

print(three("s111","s222","s333"));

class Klass
{
	int_t idata;
	
	Klass(int_t a)
	{
		idata = a;
	}
	
	int_t get()
	{
		return idata;
	}
	
	void add(int_t b)
	{
		idata += b;
	}
}

local Klass2 = class 
{
	int_t idata;
	
	constructor(int_t a)
	{
		idata = a;
	}
	
	int_t get()
	{
		return idata;
	}
	
	void add(int_t b)
	{
		idata += b;
	}
}

auto klass = new Klass(50);
print(klass->get());
klass->add(20);
print(klass->get());

auto klass2 = new Klass2(50);
print(klass2->get());
klass2->add(20);
print(klass2->get());
