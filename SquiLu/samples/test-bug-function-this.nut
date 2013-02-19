class Klass {
	k_member = null;
	
	constructor(){
		k_member = 0;
	}
	function showbug(){
		local function bug(v){
print("bug", v)
			k_member = v;
print("assignment done",  k_member);
			return "hhhh";
		}
		bug.setenv(this);
		bug("ok from here");
		local ar = ["arvalue"];
		ar.apply(bug);
	}
}

function showbug(){
	local k_member = 0;
	local function bug(v){
print("bug len", len());
print("bug", v)
		k_member = v;
print("assignment done",  k_member);
		return "hhhh";
	}
	bug("ok from here");
	local ar = ["arvalue"];
	ar.apply(bug);
}

showbug();

local k = Klass();
k.showbug();