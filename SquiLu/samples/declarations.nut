static int32_t num32 = 32;  //static is accepted but ignored
static int32_t count32; //C/C++ style declarations
print(num32, count32);

int32_t doIt(int32_t a, char_t b) //C/C++ style declarations
{
	static int32_t i =0; //static is accepted but ignored, inside functions a warning is emmited
	print("doIt", i, a, b);
}

doIt(3, "dad");

class K
{
	v1 = 0;
	v2 = 0;
	
	function f1(){}
}

struct Person //struct is handled internally as a class
{
	int32_t age; //C/C++ style declarations
	char_t name;
	int32_t weight;
};

class BaseVector {
	constructor(...)
	{
		if(vargv.len() >= 3) {
			x = vargv[0];
			y = vargv[1];
			z = vargv[2];
		}
	}
	
	
	x = 0;
	y = 0;
	z = 0;
	int32_t i32;
}

class Vector3 extends BaseVector {
	function _add(other)
	{
		if(other instanceof this.getclass())
			return ::Vector3(x+other.x,y+other.y,z+other.z);
		else
			throw "wrong parameter";
	}
	function Print()
	{
		::print(x+","+y+","+z+"\n");
	}
	
	bool_t isEmpty()
	{
		return true;
	}
}

class Vector4 : public Vector3 //C/C++ style declarations
{
	bool_t isFull()
	{
		return false;
	}
}

local v0 = Vector3(1,2,3)
local v1 = Vector3(11,12,13)
local v2 = v0 + v1;
v2.Print();
print(v2.isEmpty());

local v4 = Vector4();
print(v4.isFull());

FakeNamespace <- {
	Utils = {}
}

class FakeNamespace.Utils.SuperClass {
	constructor()
	{
		::print("FakeNamespace.Utils.SuperClass")
	}
}

local testy = FakeNamespace.Utils.SuperClass();