class EasyCurl {
	const_CURLAUTH_NONE = 0;
	const_CURLAUTH_BASIC = 0;
	const_CURLAUTH_DIGEST = 0;
	const_CURLAUTH_NEGOTIATE = 0;
	/* Deprecated since the advent of CURLAUTH_NEGOTIATE */
	const_CURLAUTH_GSSNEGOTIATE = 0;
	const_CURLAUTH_NTLM = 0;
	const_CURLAUTH_DIGEST_IE = 0;
	const_CURLAUTH_NTLM_WB = 0;
	const_CURLAUTH_ONLY = 0;
	const_CURLAUTH_ANY = 0;
	const_CURLAUTH_ANYSAFE = 0;

	const_CURLSSH_AUTH_ANY = 0;
	const_CURLSSH_AUTH_NONE = 0;
	const_CURLSSH_AUTH_PUBLICKEY = 0;
	const_CURLSSH_AUTH_PASSWORD = 0;
	const_CURLSSH_AUTH_HOST = 0;
	const_CURLSSH_AUTH_KEYBOARD = 0;
	const_CURLSSH_AUTH_AGENT = 0;
	const_CURLSSH_AUTH_DEFAULT = 0;

	const_CURLGSSAPI_DELEGATION_NONE = 0;
	const_CURLGSSAPI_DELEGATION_POLICY_FLAG = 0;
	const_CURLGSSAPI_DELEGATION_FLAG = 0;
	
	const_CURLHEADER_UNIFIED = 0;
	const_CURLHEADER_SEPARATE = 0;

	/* CURLPROTO_ defines are for the CURLOPT_*PROTOCOLS options */
	const_CURLPROTO_HTTP = 0;
	const_CURLPROTO_HTTPS = 0;
	const_CURLPROTO_FTP = 0;
	const_CURLPROTO_FTPS = 0;
	const_CURLPROTO_SCP = 0;
	const_CURLPROTO_SFTP = 0;
	const_CURLPROTO_TELNET = 0;
	const_CURLPROTO_LDAP = 0;
	const_CURLPROTO_LDAPS = 0;
	const_CURLPROTO_DICT = 0;
	const_CURLPROTO_FILE = 0;
	const_CURLPROTO_TFTP = 0;
	const_CURLPROTO_IMAP = 0;
	const_CURLPROTO_IMAPS = 0;
	const_CURLPROTO_POP3 = 0;
	const_CURLPROTO_POP3S = 0;
	const_CURLPROTO_SMTP = 0;
	const_CURLPROTO_SMTPS = 0;
	const_CURLPROTO_RTSP = 0;
	const_CURLPROTO_RTMP = 0;
	const_CURLPROTO_RTMPT = 0;
	const_CURLPROTO_RTMPE = 0;
	const_CURLPROTO_RTMPTE = 0;
	const_CURLPROTO_RTMPS = 0;
	const_CURLPROTO_RTMPTS = 0;
	const_CURLPROTO_GOPHER = 0;
	const_CURLPROTO_SMB = 0;
	const_CURLPROTO_SMBS = 0;
	const_CURLPROTO_ALL = 0;
	
	
	constructor()
	{
	}

	destructor()
	{
	}
	
	</ cfunc = "CURL *curl_easy_init(void)" />
	function easy_init() {}
	</ cfunc = "CURLcode curl_easy_setopt(CURL *curl, CURLoption option, ...)" />
	function easy_setopt(option, ...) {}
	</ cfunc = "CURLcode curl_easy_perform(CURL *curl)" />
	function perform () {}
	</ cfunc = "void curl_easy_cleanup(CURL *curl)" />
	function cleanup () {}
	</ cfunc = "CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ...)" />
	function easy_getinfo (info, ...) {}
	</ cfunc = "CURL* curl_easy_duphandle(CURL *curl)" />
	function easy_duphandle ()  {}
	</ cfunc = "void curl_easy_reset(CURL *curl)" />
	function easy_reset () : msg {}
	</ cfunc = "CURLcode curl_easy_recv(CURL *curl, void *buffer, size_t buflen, size_t *n)" />
	function easy_recv () {}
		
	</ cfunc = "CURLcode curl_easy_send(CURL *curl, const void *buffer, size_t buflen, size_t *n)" />
	function easy_send (buffer) : int {}
}

dofile("generate-cpp-class-wrapper.nut");
generateCppClassWrapper(EasyCurl, "EasyCurl");