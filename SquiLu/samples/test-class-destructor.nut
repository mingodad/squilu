class MyClass {
	_num : integer;
	
	constructor(){
		//base.constructor();
		_num = 0;
		print("constructor");
	}

	destructor(){
		//base.destructor();
		print("destructor");
	}
}

class MyDerivedClass extends MyClass {
	constructor(){
		base.constructor();
		_num = 2;
		print("derived constructor");
	}

	destructor(){
		base.destructor();
		print("derived destructor");
	}	
}

local a = MyClass();
print(a._num);
a.destructor();
local b = MyDerivedClass();
print(b._num);
b.destructor();