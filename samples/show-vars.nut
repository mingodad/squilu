function getMembersNotInBaseClass(klass, klass_members){
	local bklass = klass.getbase();
	local bklass_members = {};
	if(bklass) foreach(k,v in bklass) bklass_members[k] <- true;
	foreach(k,v in klass) {
		if(bklass_members.get(k, klass) == klass) { //not found
			klass_members.push([k,v]);
		}
	}
	klass_members.push([" [base class] -> ", gettypetag(bklass)]);
}

function showVars(avar, prefix=null){
	local isClass = type(avar) == "class";
	local isTable = type(avar) == "table";
	local  myvars=[];
	if(isClass) getMembersNotInBaseClass(avar, myvars);
	else 
	{
		foreach(k,v in avar) {
			if(isTable || isClass){
				if(avar.rawin(k)) myvars.push([k,v]);
			}
			else myvars.push([k,v]);
		}
	}
	myvars.sort(@(a,b) a[0] <=> b[0]);
	foreach(v in myvars) {
		if(prefix) print1(prefix);
		print(v[0], type(v[1]), v[1]);
		local vtype = type(v[1]);
		if(vtype == "class" || vtype == "table") showVars(v[1], prefix ? prefix + "\t" : "\t");
	}
}
showVars(this);
/*
//bug in passing array as default parameter ??????
function showVars(avar, prefix=null, myvars=[]){
	local isClass = type(avar) == "class";
	local isTable = type(avar) == "table";
	//local  myvars=[];
	foreach(k,v in avar) {
		if(isClass || isTable){
			if(avar.rawin(k)) myvars.push([k,v]);
		}
		else myvars.push([k,v]);
	}
	myvars.sort(@(a,b) a[0] <=> b[0]);
	foreach(v in myvars) {
		if(prefix) print1(prefix);
		print(v[0], type(v[1]), v[1]);
		local vtype = type(v[1]);
		if(vtype == "class" || vtype == "table") showVars(v[1], prefix ? prefix + "\t" : "\t");
	}
}
showVars(this);

function hideFromGlobals(){
	//class A {};
	//local instA = A();
	//local weakv = instA.weakref();
	local intv = 2;
	local tblv = {};
	local arrayv = [];
	local str = "";
	local coro = ::newthread(showVars);
	local function geny(n){
		for(local i=0;i<n;i+=1)
		yield i;
		return null;
	}
	local gtor=geny(10);


	local myvars = [
			//["array_delegate", arrayv.getdelegate()],
			//["class_delegate", A.getdelegate()],
			//["instance_delegate", instA.getdelegate()],
			//["closure_delegate", showVars.getdelegate()],
			//["generator_delegate", gtor.getdelegate()],
			//["number_delegate", intv.getdelegate()],
			//["float_delegate", floatv.getdelegate()],
			//["string_delegate", str.getdelegate()],
			//["table_delegate", tblv.getdelegate()],
			//["thread_delegate", coro.getdelegate()],
			//["weakref_delegate", weakv.getdelegate()],
		];

	showVars(this);
}
hideFromGlobals();
*/
