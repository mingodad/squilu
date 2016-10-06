auto bpack=sqpack.pack;
auto bpack_to_userdata=sqpack.pack_to_userdata;
auto bunpack=sqpack.unpack;
auto bpushmem=sqpack.pushmem;
auto bgetaddress=sqpack.getaddress;

local libsnappy = ffi.load(".libs/libsnappy.so");

enum snappy_status {
  SNAPPY_OK = 0,
  SNAPPY_INVALID_INPUT = 1,
  SNAPPY_BUFFER_TOO_SMALL = 2
};

/*
snappy_status snappy_compress(const char* input,
                              size_t input_length,
                              char* compressed,
                              size_t* compressed_length);

snappy_status snappy_uncompress(const char* compressed,
                                size_t compressed_length,
                                char* uncompressed,
                                size_t* uncompressed_length);

size_t snappy_max_compressed_length(size_t source_length);

snappy_status snappy_uncompressed_length(const char* compressed,
                                         size_t compressed_length,
                                         size_t* result);

snappy_status snappy_validate_compressed_buffer(const char* compressed,
                                                size_t compressed_length);
*/

auto udata = bpushmem(1024);
auto udata_address = bgetaddress(udata);
print(udata, type(udata), udata_address);

local snappy_compress_fptr = libsnappy.bind_func("i", "snappy_compress", ["s", "i", "s", "i"]);
local snappy_uncompress_fptr = libsnappy.bind_func("i", "snappy_uncompress", ["s", "i", "s", "i"]);
local snappy_max_compressed_length = libsnappy.bind_func("i", "snappy_max_compressed_length", ["i"]);
local snappy_uncompressed_length = libsnappy.bind_func("i", "snappy_uncompressed_length", ["s", "i", "i"]);

local function snappy_compress(str)
{
	auto slen = str.len();
	auto smlen = snappy_max_compressed_length(slen);
	auto compressed = bpushmem(smlen);
	auto compressed_length = bpack_to_userdata("i", smlen);

	auto status = snappy_compress_fptr(str, slen, compressed, bgetaddress(compressed_length));
	
	auto clen = bunpack(compressed_length, "i")[0];
	auto cstr = bunpack(compressed, "A" + clen)[0];
	print(status, compressed, smlen, clen, cstr.len());
	return cstr;
}

local function snappy_uncompress(str)
{
	auto slen = str.len();
	auto compressed_length = bpack_to_userdata("i", slen);
	auto status = snappy_uncompressed_length(str, slen, bgetaddress(compressed_length));
	auto clen = bunpack(compressed_length, "i")[0];
	auto uncompressed = bpushmem(clen);

	status = snappy_uncompress_fptr(str, slen, uncompressed, bgetaddress(compressed_length));
	
	clen = bunpack(compressed_length, "i")[0];
	auto cstr = bunpack(uncompressed, "A" + clen)[0];
	print(status, uncompressed, slen, clen, cstr.len());
	return cstr;
}

auto str = readfile(vargv[0]);
print(snappy_uncompress(snappy_compress(str)));

local smlen = snappy_max_compressed_length(1200);
print(smlen);