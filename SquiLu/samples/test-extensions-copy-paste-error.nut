local text = [==[
    SQ_GET_INTEGER(v, 2, x);
    SQ_GET_INTEGER(v, 3, y);
    SQ_GET_INTEGER(v, 4, w);
    SQ_GET_INTEGER(v, 5, h);
]==];

local error_re = "(SQ_GET_[^(]+%([^,]+,%s*(%d))[^\n]+%s+SQ_GET_[^(]+%([^,]+,%s*(%d)";

text.gmatch(error_re, function(m1, d1, d2){
	print(m1, d1,d2);
	return true;
});


local base_dir = "../../SquiLu-ext";
foreach(fname in sqfs.dir(base_dir)){
	if(fname.endswith(".cpp")){
		print(fname);
		local fd = file(base_dir + "/" + fname, "r");
		local fc = fd.read(fd.len());
		fd.close();
		fc.gmatch(error_re, function(m1, d1, d2){
			if(d1 == d2) print(m1, d1,d2);
			return true;
		});
	}
}