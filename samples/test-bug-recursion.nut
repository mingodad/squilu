function bugRecursion(str){
	print(str);
	if(str == "recurse")  bugRecursion("recurring with recurse");
}

local function bugRecursionLocal(str){
	print(str);
	if(str == "recurse")  bugRecursionLocal("recurring with recurse");
}

bugRecursion("dad");
bugRecursion("recurse");

bugRecursionLocal("dad");
bugRecursionLocal("recurse");