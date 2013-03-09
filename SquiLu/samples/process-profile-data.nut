local profile_data = [==[
1	1	7	0	_OP_LOAD
2	2	1193711	2266	_OP_LOADINT
6	6	796719	1509	_OP_CALL
8	8	265587	516	_OP_PREPCALLK
9	9	6	0	_OP_GETK
10	10	659839	1258	_OP_MOVE
14	14	528410	1004	_OP_GET
17	17	265574	552	_OP_ADD
18	18	262849	532	_OP_SUB
19	19	4	0	_OP_MUL
23	23	531133	993	_OP_RETURN
26	26	265566	513	_OP_LOADBOOL
27	27	134148	244	_OP_DMOVE
28	28	1364	2	_OP_JMP
29	29	266936	512	_OP_JCMP
30	30	265566	595	_OP_JZ
32	32	525688	969	_OP_GETOUTER
33	33	265566	517	_OP_NEWOBJ
34	34	528410	1018	_OP_APPENDARRAY
37	37	132782	254	_OP_INCL
48	48	2	6	_OP_CLOSURE
]==]

local profile = [];
local total_time = 0.0;
foreach(line in profile_data.split('\n')){
	//print(line);
	local rec = line.split('\t');
	rec[2] = rec[2].tointeger();
	rec[3] = rec[3].tointeger();
	total_time += rec[3];
	profile.push(rec);
}

foreach(rec in profile){
	local pct = (rec[3] / total_time) * 100;
	rec[1] = math.roundf(pct, 2);
	rec.insert(2, math.roundf(rec[3]/rec[2].tofloat(), 4));
}

profile.sort(@(a,b) b[1] <=> a[1]);

print("OP\t%time\t%call\t%ncalls\ttotal_time\tOP name");
foreach(rec in profile){
	print(rec.concat("\t"));
}
