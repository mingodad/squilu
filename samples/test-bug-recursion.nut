local print = print;
local myvar = "outer ?";
print(959876);
local function bugRecursionLocal(str, num=993.56){
	print(str, num, myvar);
	if(str == "recurse33")  bugRecursionLocal("1recurring with recurse", 959876);
	if(str == "recurse334")  bugRecursionLocal("2recurring with recurse", 959.876);
	if(str == myvar) print("str == myvar");
}

function bugRecursion(str, num=973){
	print(str, num);
	if(str == "recurse")  bugRecursion("3recurring with recurse", 95);
	return 8888.4;
}

bugRecursion("dad");
bugRecursion("recurse");

bugRecursionLocal("dad");
bugRecursionLocal("recurse");

enum bcode {ba, bb, bc};
const XX = 999;
local int_var = 983;
local float_var = 932456.3123;
local float_var2 = 0.0;
print(float_var2);
local aritVar = (XX + float_var) / 2 * int_var -3;
local d1 = Decimal(33.3);
local d2 = Decimal(2);
print(d1*d2/d1-d2);
local str_var = "DAD";
local bool_var = true;
local array_var = [9,1.5,2,3];
local table_var = {name="color"};

print(int_var)

local function funcLocal(str){
	bugRecursion("dad");
	bugRecursion("recurse");

	bugRecursionLocal("dad");
	bugRecursionLocal("recurse");
}

funcLocal("dad");
