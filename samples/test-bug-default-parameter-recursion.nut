//bug in passing array as default parameter ??????
local call_count = 0;

function showDefaultParameterRecursiveBug(str, ardflt=[]){
	ardflt.push(str);
	if(str == "recurse") showDefaultParameterRecursiveBug("again with recurse, default parameter should be an empty array but it's not");
	print(++call_count, str);
	foreach(v in ardflt) print("ardflt", call_count, v);
}

showDefaultParameterRecursiveBug("dad");
showDefaultParameterRecursiveBug("recurse");
