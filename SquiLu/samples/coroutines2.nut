local function _tokenizer(str) {
	local yield2 = suspend;
	local i = 0
	local i1,i2
	local find = function(pat) {
		local result = [0,0];
		//local rc = str.find_lua(pat,result,i)
//print(str, pat, rc, result[0], result[1])
		if(str.find_lua(pat,result,i) >= 0) {
			i1 = result[0]
			i2 = result[1]
			return true;
		}
		return false;
	}
	local token = function() {
//print(str, i, i1, i2)
		return str.slice(i,i2+1)
	}
	while (true) {
		if ( find("^%s+") ) {
			//-- ignore
		} else if( find( "^[%+%-]*%d+") ) {
			local ilast = i
			i = i2+1 //-- just after the sequence of digits
			//-- fractional part?
			local result = [0,0];
			if (str.find_lua("^%.%d+",result, i) >= 0) {
				i2 = result[1]
				i = i2+1
			}
			//-- exponent part?
			if (str.find_lua("^[eE][%+%-]*%d+", result, i) >= 0) {
				i2 = result[1]
			}
			i = ilast
			yield2(["number",str.slice(i,i2+1).tofloat()])
		} else if (find( "^[_%a][_%w]*")) {
			yield2(["iden",str.slice(i,i2+1)])
		} else if (find ([==[^"[^"]*"]==]) || find ("^'[^']*'") ) {
			//-- strip the quotes
			yield2(["string",str.slice(i+1,i2)])
		} else { //-- any other character
			if (i >= str.len()) return null;
			local ch = str[i].tochar()
			i2 = i
			yield2([ch,ch])
		}
		i = i2+1
	}
}

//for(var i=0; i < 10000; ++i) {
	local T = ::newthread(_tokenizer)
	var result = T.call([==[hello !-20.2e2 +10e1 "dolly"]==]);
	while( result ) {
		print(result[0], result[1]);
		result = T.wakeup()
	}
//}
