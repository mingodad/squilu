const RE_E_OK = 1;
const RE_E_FAIL = 2;
const RE_E_SYNTAX = 3;
const RE_E_NOTIMPL = 3;

local function re_match(re_str, test_str)
{
	local result;
	try {
		local re = regexp(re_str);
		result = (re.match(test_str) != null) ? RE_E_OK : RE_E_FAIL;
	} catch(e) {result = RE_E_SYNTAX;}
	return result;
}

local a = re_match("a{,2}b{2,4}", "aabbbb") == RE_E_OK;
a = re_match("a{0,2}b{2,4}", "aabbbb") == RE_E_FAIL;
a = re_match("0[xX][\\da-fA-F]{1,4}", "0x12a6") == RE_E_SYNTAX;
 
