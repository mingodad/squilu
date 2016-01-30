print(os.get_signal_received());
os.set_signal_received(2);
print(os.get_signal_received());

os.set_signal_received(0);
local sig_alarm = os.signal_str2int("SIGALRM");
os.signal(sig_alarm);

print(os.get_signal_received());

for(local i=0; i < 10; ++i) {
	if(i % 3) {
		os.raise("SIGALRM");
	}
	local sig_received = os.get_signal_received();
	print(i, sig_received, sig_received ? os.signal_int2str(sig_received) : "");
	os.set_signal_received(0);
	os.sleep(100);
}

os.set_signal_received(0);

local SIGINT = os.signal_str2int("SIGINT");
print(SIGINT, os.signal(SIGINT));

local SIGQUIT = os.signal_str2int("SIGQUIT");
print(SIGQUIT, os.signal(SIGQUIT));

local SIGTERM = os.signal_str2int("SIGTERM");
print(SIGTERM, os.signal(SIGTERM));

local SIGALRM = os.signal_str2int("SIGALRM");
print(SIGALRM, os.signal(SIGALRM));

local SIGHUP = os.signal_str2int("SIGHUP");
print(SIGHUP, os.signal(SIGHUP));

local run_loop = true;
while(run_loop) {
	local signal_received = os.get_signal_received();
	print(run_loop, signal_received, signal_received ? os.signal_int2str(signal_received) : "", SIGINT, SIGQUIT, SIGTERM);

	if(signal_received) {
		local sig_name = os.signal_int2str(signal_received);
		switch(sig_name) {
			case "SIGINT":
			case "SIGQUIT":
			case "SIGTERM":
				run_loop = false;
			break;
			case "SIGALRM":
				run_loop = false;
			break;
			case "SIGHUP":
				run_loop = false;
			break;
		}
	}
/*
	switch(signal_received) {
		case SIGINT:
		case SIGQUIT:
		case SIGTERM:
			run_loop = false;
		break;
		case SIGALRM:
			run_loop = false;
		break;
		case SIGHUP:
			run_loop = false;
		break;
	}
*/
	if(run_loop) {
		os.sleep(100);
	}
}

/*
run_loop = true;
os.set_signal_received(0);
while(run_loop) {
	local signal_received = os.get_signal_received();
	print(run_loop, signal_received, signal_received ? os.signal_int2str(signal_received) : "", SIGINT, SIGQUIT, SIGTERM);
	switch(signal_received) {
		case SIGINT:
		case SIGQUIT:
		case SIGTERM:
			run_loop = false;
		break;
		case SIGALRM:
			run_loop = false;
		break;
		case SIGHUP:
			run_loop = false;
		break;
	}
	if(run_loop) {
		os.sleep(100);
	} else {
		print(signal_received, os.signal_int2str(signal_received));
	}
}
*/