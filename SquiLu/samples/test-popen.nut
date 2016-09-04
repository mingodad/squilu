//local fd = assert(popen("/bin/ls -la", "r"));
local fd = popen("/bin/ls -la", "r");
local output = "";
local read_n = 1024;
local read_buf;
do
{
	read_buf = fd.read(read_n);
	output += read_buf;
} while(read_buf.len() == read_n);
print("pclose", fd.close());
print(output) // Prints the output of the command.

fd = popen("/bin/ls -la", "r");
output = fd.read_all();
print("pclose", fd.close());
print(output) // Prints the output of the command.