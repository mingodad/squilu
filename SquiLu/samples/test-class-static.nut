class AClass {
	static count = 0;
	count2 = null;
	constructor(){
		++count;
		count2 = 0;
	}
	
	function recurse(k){
		print(k);
		if(k=="recurse") recurse("Hello Again !");
	}
}

function AClass::Inc(){
	++count2;
	++AClass.count;
}

function AClass::Inc2(){
	++count2;
	++count;
}

function AClass::Inc3(){
	++count;
}

local a = AClass();
print(a.count);

a.recurse("dad");
a.recurse("recurse");

for(local i=0; i < 10; ++i){
	local c = AClass();
	++AClass.count;
	AClass.Inc3();
	c.Inc();
	c.Inc2();
	print(c.count);
}

print(AClass.count);
AClass.count = 3;
print(a.count);
AClass.count++;
print(AClass.count);
