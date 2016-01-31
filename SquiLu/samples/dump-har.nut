//local har_fn = "/home/mingo/Downloads/builder.yaml.de.har";

local dest_folder = "/tmp/har-tmp/";
os.system(format("mkdir %s", dest_folder));

local fd = file(har_fn, "r");
local fc = fd.read(fd.len());
fd.close();

function json2var(json) {
	local vm = SlaveVM();
	local slave_func = "getTable";
	
	//debug_print(json, "\n");
	//convert new data from json to squilu table for merge
	vm.compilestring(slave_func, "return " + json, true, true);
	local tbl = vm.call(true, slave_func);
	return tbl;
}

function dumpVar(vtd)
{
	local vtype = type(vtd);
	if( vtype == "table" )
	{
		foreach(k,v in vtd)
		{
			print(k, (v || "").tostring().len());
			dumpVar(v);
		}
	}
	else if( vtype == "array" )
	{
		foreach(k,v in vtd)
		{
			print(k, (v || "").tostring().len());
			dumpVar(v);
		}
	}
}


local folders_made = {};

function getPath(url)
{
	local ary = url.split('/');
	for(local i = 1, len = ary.len()-1; i < len; ++i)
	{
		//print(i,ary[i]);
		local folder = ary.slice(1, i+1).join("/");
		local cmd = format("mkdir %s", dest_folder + folder);
		if(!folders_made.rawget(folder, false))
		{
			print(cmd);
			os.system(cmd);
			folders_made.rawset(folder, true);
		}
	}
	local path = ary.slice(1).join("/");
	//print(path);
	return path;
}

function dumpContent(vhar)
{
	local entries = vhar.log.entries;
	foreach(k, ventry in entries)
	{
		local url = ventry.request.url;
		local path = getPath(url);
		if(path.endswith("/"))
		{
			path += "index.html";
		}
		if( ventry.response.content.mimeType == "text/html")
		{
			if(path.indexOf(".htm") < 0)
			{
				path = getPath(url + "/index.html");
			}
		}
		local idx = path.indexOf("@");
		if(idx < 0) idx = path.indexOf("?");
		
		if(idx > 0)
		{
			path = path.slice(0, idx);
		}
		
		local content = ventry.response.content.rawget("text", false);
		
		if(content)
		{
			if(ventry.response.content.rawget("encoding", "") == "base64")
			{
				content = base64.decode(content);
			}
			
			print(path, content.len());
			
			local full_path = dest_folder + path;
			local fdc = file(full_path, "w");
			fdc.write(content);
			fdc.close();
		}
	}
}

local har_json = json2var(fc);

//dumpVar(har_json);
dumpContent(har_json);
