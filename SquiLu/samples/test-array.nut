enum colors {blue0, white0, black0};
const car = 44;
print("globally defined or outside any function/scope", colors.white0, car);

local mar = 22;
{
	local mar = 66;
}

function doIt(&acolor){
	//const car = 33;
	const foo = 10;
	local mar = 11;
	mar = foo;
	int_t ivar = 34;
	float_t fvar = 2.5;
	fvar = ivar;
	//++car;
	print("foo", foo);
	//++foo;
	print("foo", foo);
	enum colors2 {blue, white, black};
	acolor[colors2.blue] = true;
	acolor[colors2.white] = true;
	acolor[colors2.black] = true;
	print("inner scoped overwrite const ", car);
	local function inner(){
		local mar = 35;
		print("inner scoped ", car, colors2.white);
		//car = 34;
		print("inner scoped  overwrite ", car);
	}
	inner();
	//car = 45;
}
//print(car);
print("values overwriten by innerscope", colors.white0, car);
local ar = [false, false, false];
doIt(ar);

local nar = [1,2,3,4,5,6,7,8,9];
foreach(v in nar) print(nar.bsearch(v));
print(nar.bsearch(0));
print(nar.bsearch(10));

local sar = ["1","2","3","4","5","6","7","8","9"];
foreach(v in sar) print(sar.bsearch(v));
print(sar.bsearch("0"));
print(sar.bsearch("10"));

local array_size = 1000;
ar = array(array_size);
for(local i=0; i < array_size; ++i) ar[i] = i;

local sum, start_time;

start_time = os.clock();
sum = 0;
foreach(elm in ar) sum += elm;
print("Time spent", os.clock() - start_time, sum);

start_time = os.clock();
sum = 0;
foreach(elm in ar) sum += ar[elm];
print("Time spent", os.clock() - start_time, sum);

start_time = os.clock();
sum = 0;
foreach(elm in ar) sum += ar.get(elm);
print("Time spent", os.clock() - start_time, sum);

start_time = os.clock();
sum = 0;
for(local i=0; i < array_size; ++i) sum += ar[i];
print("Time spent", os.clock() - start_time, sum);

start_time = os.clock();
sum = 0;
for(local i=0; i < array_size; ++i) sum += ar.get(i);
print("Time spent", os.clock() - start_time, sum);
