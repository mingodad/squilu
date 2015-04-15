# A powerful simple preprocessor #

Using the power of Lua compact regular expression and SquiLu scripting language we can have a simple and powerful preprocessor, the code bellow recognize SquiLu code hidden inside comments with special keywords, the resulting preprocessed file can be reprocessed again because it contains all the original content plus any extra code generated during preprocess, the extra code generated when preprocessing is surrounded by "// generated-code:begin" and "// generated-code:end" and is removed before preprocess.


```
/*SquiLu
//any SquiLu code that will be executed during preprocess fase
function mk_const(k){
  putsnl(format("const %s = \"%s\";", k.toupper(), k));
}
SquiLu*/ 

//@mk_const("dad"); //inline function call that will generate extra code
```

After preprocess:
```
/*SquiLu
//any SquiLu code that will be executed during preprocess fase
function mk_const(k){
  putsnl(format("const %s = \"%s\";", k.toupper(), k));
}
SquiLu*/ 

//@mk_const("dad"); //inline function call that will generate extra code
// generated-code:begin
const DAD = "dad";
// generated-code:end
```

Here is the preprocessor:
```
#!/home/mingo/bin/squilu
function puts(s) {
	fd.write(s);
}

function putsnl(s) {
	fd.write(s);
	fd.write("\n");
}

function preprocess(file_name){
	local fd = file(file_name, "r");
	local code = fd.read(fd.len());
	fd.close();
	
	local function escape_re(str){
		local new_str = str.gsub("[-.$%%[%]^]", "%%%1")
		return new_str
	}
	local code_generation_begin = "// generated-code:begin";
	local code_generation_end = "// generated-code:end";

	local code_generation_begin_escaped = escape_re(code_generation_begin);
	local code_generation_end_escaped = escape_re(code_generation_end);

	//print(code_generation_begin, code_generation_begin_escaped);

	local new_code = code.gsub(code_generation_begin_escaped + ".-" + code_generation_end_escaped, "");

	new_code = new_code.gsub("(//@(.-)\n)", function(m, m2) {
			return format("%s%s}====})\n%s;\nputsnl({===={\n%s", m, code_generation_begin, m2, code_generation_end)
		});


	new_code = new_code.gsub("(/%*SquiLu(.-)SquiLu%*/)", function(m, m2) {
			return format("%s}====})\n%s\nputsnl({===={", m, m2)
		});

	local buffer = blob();
	buffer.write("puts({===={");
	buffer.write(new_code);
	buffer.write("}====})");
	local sqcode = buffer.tostring();
	
	local code_func = compilestring(sqcode, "sqcode-preprocessed");

	local bak_filename = file_name + ".pp.bak";
	os.rename(file_name, bak_filename);

	::fd <- file(file_name, "w");
	code_func();
	::fd.close();

}

if(vargv.len() > 1){
	preprocess(vargv[1]);
}
```