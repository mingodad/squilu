var fd = file("lexer.nut", "r");
local txt = fd.read(fd.len());
fd.close();

var lex = new SQLexer(txt);
print(lex.first_enum_token(), lex.last_enum_token());


var tok =  lex.lex();
while(tok > 0) {
	print(
		tok,
		lex.tok2str(tok),
		lex.token_name(tok),
		lex.svalue(),
		lex.currentline(),
		lex.currentcolumn()
		);
	tok = lex.lex();
}