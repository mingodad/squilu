var fd = file("lexer.nut", "r");
local txt = fd.read(fd.len());
fd.close();

var lex = new SQLexer(txt);

local first_enum_token = lex.first_enum_token();
local last_enum_token = lex.last_enum_token();
print(first_enum_token, last_enum_token);

var gconstants = getconsttable();
for(var i=first_enum_token+1; i < last_enum_token; ++i)
{
	var tkname = lex.token_name(i);
	table_rawset(gconstants, tkname, i);
	print(tkname, i);
}


var tok =  lex.lex();
while(tok > 0) {
	local tkstr;
	if(tok > first_enum_token) tkstr = lex.tok2str(tok);
	else tkstr = tok.tochar();
	print(
		tok,
		tkstr,
		lex.token_name(tok),
		lex.svalue(),
		lex.currentline(),
		lex.currentcolumn()
		);
	tok = lex.lex();
	/*
	if(tok == TK_CLASS)
	{
		print("class found");
	}
	*/
}