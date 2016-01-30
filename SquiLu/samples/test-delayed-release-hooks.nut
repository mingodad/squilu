//local dad_utils = dad_utils;
local xx = 0;
function doIT(line){
	print(dad_utils, dad_utils.gc_scope_alert);
	local gca = dad_utils.gc_scope_alert(@(ud) print(ud), "<<Gone !>> : " + line);
	print("Done !", line);
	//gca = null;
	print(line);
}
function doIT2(line){
	local gca2 = dad_utils.gc_scope_alert(@(ud) print(ud), "<<Gone2 !>> : " + line);
	print("Done2 !", line);
	//gca2 = null;
	print(line);
}
doIT(__LINE__);
//call_delayed_release_hooks();
print("At line:", __LINE__);
doIT(__LINE__);
print("At line:", __LINE__);
local tmpn=os.tmpname();
print(tmpn);
doIT2(__LINE__);
print("At line:", __LINE__);
doIT(__LINE__);
print("At line:", __LINE__);
print(os.clock());
doIT2(__LINE__);
print("At line:", __LINE__);

for(local i=0; i<10; ++i){
	doIT(__LINE__);
print("At line:", __LINE__);
	doIT2(__LINE__);
print("At line:", __LINE__);
}
print("At line:", __LINE__);

doIT2(__LINE__);
print("At line:", __LINE__);

doIT2(__LINE__);
print("At line:", __LINE__);
