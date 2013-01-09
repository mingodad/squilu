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

local ar = vm.call(true, "split", "1,2,3,4,5", ",");
print(vm.call(true, "split", "1,2,3,4,5", ","));

local globals = getroottable();
if(!globals.get("_slave_", false)){
	print("I'm not a slave !");
	//vm.dofile("test-slave-vm.nut");
	//vm.dofile("loops.nut");
}
else print("I'm a slave !");

local slave_func = "say_something";
vm.compilestring(slave_func, "::print(\"ciao\")");
vm.call(false, slave_func);

