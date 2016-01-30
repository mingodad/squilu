class MyClass {
	_num : integer;
	
	constructor(){
		//base.constructor();
		_num = 1;
		print("constructor", this);
	}

	destructor(){
		//base.destructor();
		print("destructor", this);
	}
}

class MyDerivedClass extends MyClass {
	constructor(){
		base.constructor();
		_num = 2;
		print("derived constructor", this);
	}

	destructor(){
		base.destructor();
		print("derived destructor", this);
	}	
}
{
local a = MyClass();
print(a._num, type(a));
//a.destructor();
local b = MyDerivedClass();
print(b._num, type(b));
//b.destructor();
}

print("Done");