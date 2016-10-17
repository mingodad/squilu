//auto str = " this code 10.89.973.8.3.00.34-8 is special 23.456-2 car";
auto str = " this code 10.89.973.8.3.00.34/8 is special 23.456/2 car";

//auto pcre = sqpcre(@"(\d+[.,\-])+\d+");
//auto pcre = sqpcre(@"(?:\d+[.,\-/])+\d+");
auto pcre = sqpcre(@"(?:\d+[.,\-/])+\d+");
pcre.study(pcre.STUDY_JIT_COMPILE);
print(pcre);
print(pcre.version());

print("match", pcre.match(str));

pcre.gmatch(str,
	function(...){
		print("gmatch vargv.len()", vargv.len());
		foreach(idx, elm in vargv) print(idx, elm);
		return true;
	});
	
auto new_str = pcre.gsub(str, "@$0@");
print(new_str);

auto result = [];
auto rc, start_pos;

auto max_loop = 1;

auto start_time = os.getmillicount();
for(auto i=0; i < max_loop; ++i)
{
	start_pos = 0;
	while( (rc = pcre.exec(str, result, start_pos)) > 0)
	{
		print(rc, result.len());

		if(rc > 0)
		{
			//foreach(idx, elm in result) 
			print(str.slice(result[0], result[1]));
			start_pos = result[1] + 1;
		}
	}
}
print("Spent time", os.getmillicount() - start_time);

/*
start_time = os.getmillicount();
for(auto i=0; i < max_loop; ++i)
{
	start_pos = 0;
	while( (rc = str.find_lua("(%d[,.%-])", result, start_pos)) > 0)
	{
		print(rc, result.len());

		if(rc > 0)
		{
			foreach(idx, elm in result) print(idx, elm, str.slice(result[0], result[1]));
			start_pos = result[1] + 1;
		}
	}
}
print("Spent time", os.getmillicount() - start_time);
*/
