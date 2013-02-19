local line;
while((line = stdin.read_line())) print(line);

local str = "localhost:8080";
local t = str.find(":");
local host = str.substr(0, t);
local port = str.substr(t+1);
print(host, port);

function format_test(text, err){
	local ret = "\n";
	local line_number = 1;
	local line_err = 0, line_str;
	err.gmatch(".*:(%d+):(.*)", function(err, str){
		line_err = err;
		line_str = str;
		return false;
	});
	foreach( line in text.split("\n")) {
		if (line_number == line_err.tointeger())
			ret += format("-->   %d. %s (%s)\n", line_number, line, line_str);
		else
			ret += format("% 8d. %s\n", line_number, line);
		line_number = line_number + 1;
	}
	return ret;
}


function test(text){
//compilestring(text)();
try{
	print(34);
	print(35);
	//throw("In line " + __LINE__);
	compilestring(text)();
}
catch(e)
{
	print(e);
	foreach(k, v in get_last_stackinfo()){
		print(k,v);
	}

}
	local chunk = compilestring(text);
	if (chunk == null){
		print(format([==[
--------------------------------------------------------------- 
%s
loadstring error: '%s'
--------------------------------------------------------------- 
]==], format_test(text, err), err))
		return false;
	}

	try {
		chunk();
	}
	catch(e){
		print(format([==[
--------------------------------------------------------------- 
%s
call error: '%s'
--------------------------------------------------------------- 
]==], format_test(text, e), e));
		//return false;
	}

	return true;
}
	test([==[
		local p = SqRs232();
		p.open("/dev/ttyS0");
	]==]);
print(23);