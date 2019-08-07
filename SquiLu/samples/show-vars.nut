local base_class_key = " [base class] -> ";
local function getMembersNotInBaseClass(klass, klass_members){
	local bklass = klass.getbase();
	local bklass_members = {};
	if(bklass) foreach(k,v in bklass) bklass_members[k] <- true;
	foreach(k,v in klass) {
		if(table_get(bklass_members, k, klass) == klass) { //not found
			klass_members.push([k,v]);
		}
	}
	klass_members.push([base_class_key, gettypetag(bklass)]);
}

/** Make a human readable representation of a bit encoded integer.
@param pc is an array of bit encoded integers 
@return string representing the parameters
*/
local function getParamsCheck(pc){
	local mask_values = {
		["NULL"]		=	0x00000001,
		INTEGER		=	0x00000002,
		FLOAT		=	0x00000004,
		BOOL		=	0x00000008,
		STRING		=	0x00000010,
		TABLE		=	0x00000020,
		ARRAY		=	0x00000040,
		USERDATA	=	0x00000080,
		CLOSURE		=	0x00000100,
		NATIVECLOSURE	= 0x00000200,
		GENERATOR	=	0x00000400,
		USERPOINTER	=	0x00000800,
		THREAD		=	0x00001000,
		FUNCPROTO	=	0x00002000,
		CLASS		=	0x00004000,
		INSTANCE	=	0x00008000,
		WEAKREF		=	0x00010000,
		OUTER		=	0x00020000,
	}
	local ANY = -1;
	local tmp = [];
	foreach(v in pc){
		if(v == ANY) {
			tmp.push("ANY");
			continue;
		}
		local m = [];
		foreach(k, mv in mask_values){
			if(mv & v) m.push(k);
		}
		tmp.push(m.concat("|"));
	}
	return "(" + tmp.concat(" , ") + ")";
}

/*local do not allow recursion*/
local function showVars(avar, myvars, prefix=null){
	local isClass = type(avar) == "class";
	local isTable = type(avar) == "table";

	if(isClass) getMembersNotInBaseClass(avar, myvars);
	else 
	{
		foreach(k,v in avar) {
			/* class and instance do not work with this code
			if(isTable){
				if(avar.rawin(k)) {
					myvars.push([k,v]);
				}
			}
			else */
			myvars.push([k,v]);
		}
	}
	myvars.sort(@(a,b) a[0] <=> b[0]);
	foreach(v in myvars) {
		local vtype = type(v[1]);
		if(prefix) print1(prefix);
		local vvalue = "";
		try { 
			if(vtype == "function"){
				local infos = v[1].getinfos();
				if(infos.native){
					vvalue = getParamsCheck(infos.typecheck);
				}
			}
			else
			{
				vvalue = v[1].tostring(); 
				if(vvalue[0] == '(') vvalue = "";
			}
		} catch(e) {}
		if(v[0] == base_class_key) {
			vtype = "";
			if(vvalue == "") vvalue = "NONE";
		}
		print(v[0], vtype, vvalue);
		if(vtype == "class" || vtype == "table") showVars(v[1], [], prefix ? prefix + "\t" : "\t");
	}
}
//showVars(this, []);

local function hideFromGlobals(){
	/*
	There is a bug in the language that when a class implements the metamethod _get 
	that expects other types than string  make impossible to call default delegate functions
	like here blob/file/std_stream implements _get expecting a number (integer|float).
	local instBlob = blob();
	local weakv = instBlob.weakref();
	*/
	local inst_klass = Decimal(); //SlaveVM(1024);
	local weakv = inst_klass.weakref();
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
			["array_delegate", arrayv.getdelegate()],
			["class_delegate", getdefaultdelegate(blob)],
			["instance_delegate", getdefaultdelegate(inst_klass)],
			["closure_delegate", getdefaultdelegate(showVars)],
			["generator_delegate", getdefaultdelegate(gtor)],
			["number_delegate", getdefaultdelegate(intv)],
			["string_delegate", str.getdelegate()],
			["table_delegate", getdefaultdelegate(tblv)],
			["thread_delegate", getdefaultdelegate(coro)],
			["weakref_delegate", getdefaultdelegate(weakv)],
		];

	showVars(this, myvars);
	
	print("\n<<<Constants>>>\n")
	local const_array = [];
	foreach(k,v in getconsttable()){
		const_array.push([k,v]);
	}
	const_array.sort(@(a,b) a[0] <=> b[0]);
	foreach(v in const_array){
		print(v[0], v[1]);
	}
}
hideFromGlobals();

