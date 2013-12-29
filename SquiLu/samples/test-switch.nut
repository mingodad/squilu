local n = 10;
local b = 10;

for(local i=0; i < 5; ++i) {
	print(i, n, b);
	switch(n) {
		case 1:
			print(n);
		break;
		case -2:
			print(n);
		break;
		case 10:
			print(n);
		break;
	}
}
