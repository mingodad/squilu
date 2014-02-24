class Lexer {
	pos = 0;
	line = 0;
	buf = null;
	buflen = 0;

	// Operator table, mapping operator -> token name
	static optable = {
		"+":  "PLUS",
		"-":  "MINUS",
		"*":  "MULTIPLY",
		".":  "PERIOD",
		"\\": "BACKSLASH",
		":":  "COLON",
		"%":  "PERCENT",
		"|":  "PIPE",
		"!":  "EXCLAMATION",
		"?":  "QUESTION",
		"#":  "POUND",
		"&":  "AMPERSAND",
		";":  "SEMI",
		",":  "COMMA",
		"(":  "L_PAREN",
		")":  "R_PAREN",
		"<":  "L_ANG",
		">":  "R_ANG",
		"{":  "L_BRACE",
		"}":  "R_BRACE",
		"[":  "L_BRACKET",
		"]":  "R_BRACKET",
		"=":  "EQUALS"
	};


    // Initialize the Lexer's buffer. This resets the lexer's internal
    // state and subsequent tokens will be returned starting with the
    // beginning of the new buffer.
    function input(buf)
    {
        this.pos = 0;
	this.line = 0;
        this.buf = buf;
        this.buflen = buf.len();
    }

    // Get the next token from the current buffer. A token is an object with
    // the following properties:
    // - name: name of the pattern that this token matched (taken from rules).
    // - value: actual string value of the token.
    // - pos: offset in the current buffer where the token starts.
    //
    // If there are no more tokens in the buffer, returns null. In case of
    // an error throws Error.
    function token()
    {
        var _isnewline = function(c)
        {
            return c === '\r' || c === '\n';
        }

        var _isdigit = function (c)
        {
            return c >= '0' && c <= '9';
        }

        var _isalpha = function (c)
        {
            return (c >= 'a' && c <= 'z') ||
                   (c >= 'A' && c <= 'Z') ||
                   c === '_' || c === '$';
        }

        var _isalphanum = function(c)
        {
            return (c >= 'a' && c <= 'z') ||
                   (c >= 'A' && c <= 'Z') ||
                   (c >= '0' && c <= '9') ||
                   c === '_' || c === '$';
        }

        var _process_number = function()
        {
            var endpos = this.pos + 1;
            while (endpos < this.buflen &&
                    _isdigit(this.buf[endpos]))
            {
                endpos++;
            }

            var tok = {
		name: "NUMBER",
		value:  this.buf.slice(this.pos, endpos),
		pos: this.pos, line: this.line
            };
            this.pos = endpos;
            return tok;
        }

        var _process_comment = function ()
        {
            var endpos = this.pos + 2;
            // Skip until the end of the line
            var c = this.buf[this.pos + 2];
            while (endpos < this.buflen &&
                    !_isnewline(this.buf[endpos]))
            {
                endpos++;
            }

            var tok = {
		name: "COMMENT",
		value: this.buf.slice(this.pos, endpos),
		pos: this.pos, line: this.line
            };
            this.pos = endpos + 1;
	    ++this.line;
            return tok;
        }

        var _process_identifier = function()
        {
            var endpos = this.pos + 1;
            while (endpos < this.buflen &&
                    _isalphanum(this.buf[endpos]))
            {
                endpos++;
            }

            var tok = {
		name: "IDENTIFIER",
		value: this.buf.slice(this.pos, endpos),
		pos:  this.pos, line: this.line
            };
            this.pos = endpos;
            return tok;
        }

        var _process_quote = function(quote)
        {
            // this.pos points at the opening quote. Find the ending quote.
            var end_index = this.buf.find(quote, this.pos + 1);
	    while((this.buf[end_index-1] === '\\') && (this.buf[end_index-2] !== '\\')  ){
		end_index = this.buf.find(quote, end_index + 1);
	    }

            if (end_index === -1)
            {
                throw ("Unterminated quote at " + this.pos);
            }
            else
            {
                var tok = {
			name: "QUOTE",
			value: this.buf.slice(this.pos, end_index + 1),
			pos: this.pos, line: this.line
                };
                this.pos = end_index + 1;
                return tok;
            }
        }

        var _skipnontokens = function()
        {
            while (this.pos < this.buflen)
            {
                var c = this.buf[this.pos];
                if (c == ' ' || c == '\t')
                {
                    ++this.pos;
                }
                else if (c == '\r' || c == '\n')
                {
                    ++this.pos;
		    ++this.line;
                }
                else
                {
                    break;
                }
            }
        }

        _skipnontokens();
        if (this.pos >= this.buflen)
        {
            return null;
        }

        // The char at this.pos is part of a real token. Figure out which.
        var c = this.buf[this.pos];

        // '/' is treated specially, because it starts a comment if followed by
        // another '/'. If not followed by another '/', it's the DIVIDE
        // operator.
        if (c === '/')
        {
            var next_c = this.buf[this.pos + 1];
            if (next_c === '/')
            {
                return _process_comment();
            }
            else
            {
                return {name: "DIVIDE", value: '/', pos: this.pos++, line: this.line};
            }
        }
        else
        {
            // Look it up in the table of operators
            var op = this.optable.get(c.tochar(), false);
            if (op)
            {
                return {name: op, value: c, pos: this.pos++, line: this.line};
            }
            else
            {
                // Not an operator - so it's the beginning of another token.
                if (_isalpha(c))
                {
                    return _process_identifier();
                }
                else if (_isdigit(c))
                {
                    return _process_number();
                }
                else if (c === '"' || c === '\'')
                {
                    return _process_quote(c.tochar());
                }
                else
                {
                    throw ("Token error at " + this.pos);
                }
            }
        }
    }
}

var txt = "var lex = new Lexer(23, \"dad\");";
var lex = new Lexer();
var fd = file("lexer.nut", "r");
txt = fd.read(fd.len());
fd.close();

lex.input(txt);
var tok =  lex.token();
while(tok) {
	print(tok.name, tok.value, tok.line+1, tok.pos);
	tok = lex.token();
}
