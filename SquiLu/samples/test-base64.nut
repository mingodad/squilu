local credentials = format("\x00%s\x00%s", 
			"oneUser", "onePasswd");
print("credentials", credentials.len(), credentials);
credentials = base64.encode(credentials);
print("credentials", credentials.len(), credentials);

local b = blob();

b.write("Domingo");
print(b.tostring());

local ia = 3;
local &ria = ia;

print(ia, ria);
