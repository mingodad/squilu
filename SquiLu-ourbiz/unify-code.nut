local included = {};

function unify_code(fname){
	if(included.get(fname, false)) return "";
	else included[fname] <- true;
	
	local fd = file(fname, "r");
	local fc = fd.read(fd.len());
	fd.close();
	local last_pos = 0, start_pos = 0, end_pos = 0;
	local result = fc.gsub("(dofile%(\"([^\"]+)\"%);)", function(m1, m2){
		print(m1, m2);
		return format("dummy();//%s\n{\n%s\n}\n", m1, unify_code(m2));
	});
	return result;
}

//print(unify_code("ourbiz-fltk.nut"));

if (vargv.len() > 0){ 
	local infile = vargv[0], outfile = "-";
	if(vargv.len() > 1) outfile = vargv[1];
	local unified = unify_code(infile);
	if(outfile == "-") print(unified);
	else
	{
		local fd = file(outfile, "w");
		fd.write(unified, unified.len());
		fd.close();
	}
}