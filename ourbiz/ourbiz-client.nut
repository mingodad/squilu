class AuthDigest {
	_nc = null;
	_cnonce = null;
	_username = null;
	_password = null;
	_authenticate = null;
	_realm = null;
	_qop = null;
	_nonce = null;
	_opaque = null;
	
	constructor(username =null, password = null){
		_nc = 0;
		_cnonce = os.time();
		credentials(username, password);
	}
	function credentials(username, password){
		_username = username;
		_password = password;
	}
	function okToAuthDigest(){
		return _authenticate && _authenticate.len() > 0;
	}
	function auth_digest(method, uri){
		if(!okToAuthDigest()) throw "Trying to auth digest without authentication.";
		local ha1, ha2, ha, ncbuf;
		ncbuf = format("%08x", ++_nc);
		local cnoncebuf = format("%012d", ++_cnonce);
		local _sep = ":";
		ha1 = md5(_username, _sep, _realm, _sep, _password);
		ha2 = md5(method, _sep, uri);
		ha = md5(ha1, _sep, _nonce, _sep, ncbuf, _sep, cnoncebuf, _sep, _qop, _sep, ha2);
		local result = format([==[
Digest username="%s", realm="%s", nonce="%s", uri="%s", qop="%s", nc="%s", cnonce="%s", response="%s"]==], _username, _realm, _nonce, uri, _qop, ncbuf, cnoncebuf, ha);
		if(_opaque && _opaque.len()){
			result = format("%s, opaque=\"%s\"", result, _opaque);
		}
		return result;
	}

	function parse_authenticate(szAuthenticate){
		_authenticate = szAuthenticate;
		_realm = null;
		_qop = null;
		_nonce = null;
		_opaque = null;
		local function checkKV(k,v){
			if(v[0] == '"') v = v.slice(1, -2);
			if(k == "realm") _realm = v;
			else if(k == "qop") _qop = v;
			else if(k == "nonce") _nonce = v;
			else if(k == "opaque") _opaque = v;
			return true;
		}
		checkKV.setenv(this);
		szAuthenticate.gmatch("(%w+)%s*=%s*(%S+),", checkKV);
		printf("%s, %s, %s, %s\n", _realm || "?", _qop || "?", _nonce || "?", _opaque || "?");
	}
}

//local auth = AuthDigest();
//auth.parse_authenticate([==[Digest username="mingote", realm="ourbiz.dadbiz.es", nonce="7f1de29e6da19d22b51c68001e7e0e54", uri="/DB/GetList?list=sales_tax_rates", response="08cfa598e5945d3aada09308017da216", qop=auth, nc=00000002, cnonce="bf5f336c1827bbb2"]==]);

dofile("happyhttp.nut");

test_happyhttp();