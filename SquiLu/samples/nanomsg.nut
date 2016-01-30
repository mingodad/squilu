class NanoMsg {
	/*  Constants that are returned in `ns` member of nn_symbol_properties */
	const_NN_NS_NAMESPACE = 0;
	const_NN_NS_VERSION = 1;
	const_NN_NS_DOMAIN = 2;
	const_NN_NS_TRANSPORT = 3;
	const_NN_NS_PROTOCOL = 4;
	const_NN_NS_OPTION_LEVEL = 5;
	const_NN_NS_SOCKET_OPTION = 6;
	const_NN_NS_TRANSPORT_OPTION = 7;
	const_NN_NS_OPTION_TYPE = 8;
	const_NN_NS_OPTION_UNIT = 9;
	const_NN_NS_FLAG = 10;
	const_NN_NS_ERROR = 11;
	const_NN_NS_LIMIT = 12;
	const_NN_NS_EVENT = 13;

	/*  Constants that are returned in `type` member of nn_symbol_properties */
	const_NN_TYPE_NONE = 0;
	const_NN_TYPE_INT = 1;
	const_NN_TYPE_STR = 2;

	/*  Constants that are returned in the `unit` member of nn_symbol_properties  */
	const_NN_UNIT_NONE = 0;
	const_NN_UNIT_BYTES = 1;
	const_NN_UNIT_MILLISECONDS = 2;
	const_NN_UNIT_PRIORITY = 3;
	const_NN_UNIT_BOOLEAN = 4;
	
	const_NN_MSG = -1;
	
	/*  SP address families. */
	const_AF_SP = 1;
	const_AF_SP_RAW = 2;

	/*  Max size of an SP address. */
	const_NN_SOCKADDR_MAX = 128;

	/*  Socket option levels: Negative numbers are reserved for transports,
	    positive for socket types. */
	const_NN_SOL_SOCKET = 0;

	/*  Generic socket options (NN_SOL_SOCKET level). */
	const_NN_LINGER = 1;
	const_NN_SNDBUF = 2;
	const_NN_RCVBUF = 3;
	const_NN_SNDTIMEO = 4;
	const_NN_RCVTIMEO = 5;
	const_NN_RECONNECT_IVL = 6;
	const_NN_RECONNECT_IVL_MAX = 7;
	const_NN_SNDPRIO = 8;
	const_NN_RCVPRIO = 9;
	const_NN_SNDFD = 10;
	const_NN_RCVFD = 11;
	const_NN_DOMAIN = 12;
	const_NN_PROTOCOL = 13;
	const_NN_IPV4ONLY = 14;
	const_NN_SOCKET_NAME = 15;

	/*  Send/recv options. */
	const_NN_DONTWAIT = 1;

	/*  Ancillary data. */
	const_PROTO_SP = 1;
	const_SP_HDR = 1;
	
	/*  Socket mutliplexing support. */
	const_NN_POLLIN = 1;
	const_NN_POLLOUT = 2;	
	
	</ cfunc = "int nn_socket (int domain, int protocol)" />
	constructor(int_domain, int_protocol)
	{
	}

	</ cfunc = "int nn_close (int s)" />
	destructor()
	{
	}
	
	</ cfunc = "int nn_errno (void)" />
	function errno() {}
	</ cfunc = "const char *nn_strerror (int errnum)" />
	function strerror(int_errnum) {}
	</ cfunc = "const char *nn_symbol (int i, int *value)" />
	function symbol (int_i, value : intPtr) : string {}
	</ cfunc = "int nn_symbol_info (int i, struct nn_symbol_properties *buf, int buflen)" />
	function symbol_info (int_i) : nn_symbol_properties {}
	</ cfunc = "void nn_term (void)" />
	function term () {}
	</ cfunc = "void *nn_allocmsg (size_t size, int type)" />
	function allocmsg (int_size, int_the_type) : msg {}
	</ cfunc = "void *nn_reallocmsg (void *msg, size_t size)" />
	function reallocmsg (msg : msg, size : size_t) : msg {}
	</ cfunc = "int nn_freemsg (void *msg)" />
	function freemsg (msg) {}
		
	</ cfunc = "int nn_socket (int domain, int protocol)" />
	function socket (int_domain, int_protocol) : int {}
	</ cfunc = "int nn_close (int s)" />
	function close () : int {}
	</ cfunc = "int nn_setsockopt (int s, int level, int option, const void *optval, size_t optvallen)" />
	function setsockopt (int_level, int_option, optval : voidPtr,  optvallen : size_t) : int {}
	</ cfunc = "int nn_getsockopt (int s, int level, int option, void *optval, size_t *optvallen)" />
	function getsockopt (int_level, int_option, optval : voidPtr, optvallen: size_tPtr) : int {}
	</ cfunc = "int nn_bind (int s, const char *addr)"/>
	function bind (str_addr) : int {}
	</ cfunc = "int nn_connect (int s, const char *addr)" />
	function connect (str_addr) : int {}
	</ cfunc = "int nn_shutdown (int s, int how)" />
	function shutdown (int_how) : int {}
	</ cfunc = "int nn_send (int s, const void *buf, size_t len, int flags)" />
	function send (str_buf, int_flags) : int {}
	</ cfunc = "int nn_recv (int s, void *buf, size_t len, int flags)" />
	function recv (int_flags) : string {}
	</ cfunc = "int nn_sendmsg (int s, const struct nn_msghdr *msghdr, int flags)" />
	function sendmsg (msghdr : nn_msghdr, flags : int) : int {}
	</ cfunc = "int nn_recvmsg (int s, struct nn_msghdr *msghdr, int flags)" />
	function recvmsg (int_flags) : nn_msghdr {}
		
	</ cfunc = "int nn_poll (struct nn_pollfd *fds, int nfds, int timeout)" />
	function pollfd(fds, int_timeout) {}
		
	</ cfunc = "int nn_device (int s1, int s2)" />
	function device(int_s1, int_s2) {}
		
	</ cfunc = "int nn_tcpmuxd (int port)" />
	function tcpmuxd(int_port) {}
}

dofile("generate-cpp-class-wrapper.nut");
generateCppClassWrapper(NanoMsg, "NanoMsg");