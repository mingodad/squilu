function showVars(avar, prefix=null){
	local isClass = type(avar) == "class";
	local isTable = type(avar) == "table";
	local myvars = [];
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
