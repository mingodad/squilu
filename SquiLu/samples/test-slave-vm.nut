local vm = SlaveVM();
vm.set("_slave_", true);

print(vm);
print(vm.get("_version_"));
vm.set("dad", "Domingo");
print(vm.get("dad"));

print(vm.get("dad2", "dad2 not found"));

vm.rate = 33.7;
print(vm.rate, vm.get("rate"));

local tbl = {
	name="config",
	age=32,
	active=true,
	total=45.98,
	colors=[1,2,3,4,5],
}

vm.mc = tbl;

local mc = vm.get("mc");

foreach(k,v in mc) print(k,v);

local ar = "1,2,3,4,5".split(',');
print("1,2,3,4,5".split(','));

local globals = getroottable();
if(!globals.get("_slave_", false)){
	print("I'm not a slave !");
	//vm.dofile("test-slave-vm.nut");
	//vm.dofile("loops.nut");
}
else print("I'm a slave !");

/*
local slave_func = "say_something";
vm.compilestring(slave_func, "::print(\"ciao\")");
vm.call(false, slave_func);
*/

try {
	local slave_func = "getTable";
	vm.compilestring(slave_func, [==[return {"nodes":[{"_id":"5206163192518f6f21000180","tag":"body","classes":[],"@class":[],"children":["1d4048ee-0942-df4c-a506-5ee3e3ca8b6d"],"data":{"attr":{},"clearfix":false}},{"_id":"1d4048ee-0942-df4c-a506-5ee3e3ca8b6d","tag":"img","classes":[],"@class":[],"@src":"/wf/design/blank/comment.png","children":[],"data":{"attr":{"src":"/wf/design/blank/comment.png"},"image":{"width":100,"height":35,"size":1943,"fileName":"comment.png","_id":"dad234"}}}],"dropCount":0,"lastSynced":null}]==]);
	local slave_tbl = vm.call(true, slave_func);
	foreach(k,v in slave_tbl) print(k,v);
}
catch(e){
	print(e);
}
