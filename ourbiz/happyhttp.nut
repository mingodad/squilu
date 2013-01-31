/*
 * HappyHTTP - a simple HTTP library
 * Version 0.1
 *
 * Copyright (c) 2006 Ben Campbell
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software in a
 * product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not
 * be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 *Ported to SquiLu http://code.google.com/p/squilu/ by Domingo Alvarez Duarte
 */
 
// HTTP status codes
enum HTTP_status_code {
	// 1xx informational
	CONTINUE = 100,
	SWITCHING_PROTOCOLS = 101,
	PROCESSING = 102,

	// 2xx successful
	OK = 200,
	CREATED = 201,
	ACCEPTED = 202,
	NON_AUTHORITATIVE_INFORMATION = 203,
	NO_CONTENT = 204,
	RESET_CONTENT = 205,
	PARTIAL_CONTENT = 206,
	MULTI_STATUS = 207,
	IM_USED = 226,

	// 3xx redirection
	MULTIPLE_CHOICES = 300,
	MOVED_PERMANENTLY = 301,
	FOUND = 302,
	SEE_OTHER = 303,
	NOT_MODIFIED = 304,
	USE_PROXY = 305,
	TEMPORARY_REDIRECT = 307,

	// 4xx client error
	BAD_REQUEST = 400,
	UNAUTHORIZED = 401,
	PAYMENT_REQUIRED = 402,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	NOT_ACCEPTABLE = 406,
	PROXY_AUTHENTICATION_REQUIRED = 407,
	REQUEST_TIMEOUT = 408,
	CONFLICT = 409,
	GONE = 410,
	LENGTH_REQUIRED = 411,
	PRECONDITION_FAILED = 412,
	REQUEST_ENTITY_TOO_LARGE = 413,
	REQUEST_URI_TOO_LONG = 414,
	UNSUPPORTED_MEDIA_TYPE = 415,
	REQUESTED_RANGE_NOT_SATISFIABLE = 416,
	EXPECTATION_FAILED = 417,
	UNPROCESSABLE_ENTITY = 422,
	LOCKED = 423,
	FAILED_DEPENDENCY = 424,
	UPGRADE_REQUIRED = 426,

	// 5xx server error
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	BAD_GATEWAY = 502,
	SERVICE_UNAVAILABLE = 503,
	GATEWAY_TIMEOUT = 504,
	HTTP_VERSION_NOT_SUPPORTED = 505,
	INSUFFICIENT_STORAGE = 507,
	NOT_EXTENDED = 510,
};

//-------------------------------------------------
// Connection
//
// Handles the socket connection, issuing of requests and managing
// responses.
// ------------------------------------------------

enum Connection_State { IDLE, REQ_STARTED, REQ_SENT };
class HappyHttpConnection {
	m_Port = null;
	m_Host = null;
	m_State = null;
	m_Sock = null;
	m_Buffer = null; // lines of request
	m_Outstanding = null; // responses for outstanding requests
	
	
	// doesn't connect immediately
	constructor(host, port){
		m_Host = host;
		m_Port = port;
		m_Buffer = [];
		m_Outstanding = [];
		m_State =Connection_State.IDLE;
	}

	// Don't need to call connect() explicitly as issuing a request will
	// call it automatically if needed.
	// But it could block (for name lookup etc), so you might prefer to
	// call it in advance.
	function connect(){
		m_Sock = socket.tcp();
		m_Sock.connect(m_Host, m_Port);
		m_Sock.settimeout(0.01);
	}

	// close connection, discarding any pending requests.
	function close(){
		m_Sock.close();
		m_Sock = null;
		m_Outstanding.clear();
	}

	// Update the connection (non-blocking)
	// Just keep calling this regularly to service outstanding requests.
	function pump(milisec=10) { //10 miliseconds to prevent high cpu load
		if( m_Outstanding.empty() ) return;		// no requests outstanding
		assert( m_Sock != null );	// outstanding requests but no connection!

		if( !datawaiting( m_Sock, milisec) ) return;	// recv will block

		local rc = m_Sock.receive(2048);
		switch(rc[1]){
			case socket.IO_DONE:
			case socket.IO_TIMEOUT:
				break;
			default:
				if(rc[0].len() == 0) throw(format("socket io error %d", rc[1]));
		}
		local buf = rc[0];
		local a = buf.len();
		if( a== 0 )
		{
			// connection has closed
			local r = m_Outstanding[0];
			r->notifyconnectionclosed();
			assert( r->completed() );
			m_Outstanding.remove(0);

			// any outstanding requests will be discarded
			close();
		}
		else
		{
			local used = 0;
			while( used < a && !m_Outstanding.empty() )
			{
				local r = m_Outstanding[0];
				local u = r->pump( buf, used, a-used );

				// delete response once completed
				if( r->completed() )	m_Outstanding.remove(0);
				used += u;
			}

			// NOTE: will lose bytes if response queue goes empty
			// (but server shouldn't be sending anything if we don't have
			// anything outstanding anyway)
			assert( used == a );	// all bytes should be used up by here.
		}
	}

	// any requests still outstanding?
	function outstanding() { return m_Outstanding && !m_Outstanding.empty(); }

	// ---------------------------
	// high-level request interface
	// ---------------------------

	// method is "GET", "POST" etc...
	// url is only path part: eg  "/index.html"
	// headers is array of name/value pairs, terminated by a null-ptr
	// body & bodysize specify body data of request (eg values for a form)
	function request( method, url, headers=null, body=null, bodysize=0){
		local gotcontentlength = false;	// already in headers?

		// check headers for content-length
		// TODO: check for "Host" and "Accept-Encoding" too
		// and avoid adding them ourselves in putrequest()
		if( headers ) gotcontentlength = headers.get("content-length", false);

		putrequest( method, url );

		if( body && !gotcontentlength ) putheader( "Content-Length", bodysize );

		if( headers ) foreach(name, value in headers) putheader( name, value );

		endheaders();

		if( body ) send( body, bodysize );
	}

	// ---------------------------
	// low-level request interface
	// ---------------------------

	// begin request
	// method is "GET", "POST" etc...
	// url is only path part: eg  "/index.html"
	function putrequest( method, url ){
		if( m_State != Connection_State.IDLE ) throw ("Request already issued" );
		if( !method || !url ) throw ( "Method and url can't be NULL" );

		m_State = Connection_State.REQ_STARTED;

		local req = format("%s %s HTTP/1.1", method, url);
		m_Buffer.push( req );

		putheader( "Host", m_Host );	// required for HTTP1.1

		// don't want any fancy encodings please
		putheader("Accept-Encoding", "identity");

		// Push a new response onto the queue
		local r = new HappyHttpResponse( method, url, this );
		m_Outstanding.push( r );
	}

	// Add a header to the request (call after putrequest() )
	function putheader( header, value ){
		if( m_State != Connection_State.REQ_STARTED ) throw ( "putheader() failed" );
		m_Buffer.push( format("%s: %s", header, value.tostring()) );	
	}

	// Finished adding headers, issue the request.
	function endheaders(){
		if( m_State != Connection_State.REQ_STARTED ) throw "Cannot send header";
		m_State = Connection_State.IDLE;
		m_Buffer.push( "\r\n" ); //for double "\r\n\r\n"
		local msg = m_Buffer.concat("\r\n");
		m_Buffer.clear();
		send( msg , msg.len() );
	}

	// send body data if any.
	// To be called after endheaders()
	function send( buf, numbytes ){
		if( !m_Sock ) connect();
		local n = m_Sock.send( buf , 0, numbytes );
		//print(m_Sock.getfd(), n, buf);
		if(n != numbytes) throw("Could not send ");
	}

	// return true if socket has data waiting to be read
	static function datawaiting( sock , milisec=0)
	{
		try {
			local rc = socket.select( [sock], [], milisec ? milisec / 1000.0 : 0);
			local rar = rc[0];
			if(rar.len() == 1 && rar[0] == sock) return true;
			return false;
		}
		catch(e){
			if(e == "timeout") return false;
			else throw(e);
		}
	}

    function response_begin( r ) {}
    function response_data( r, data, data_idx, numbytes ) {}
    function response_complete( r ) {}
};

enum Response_state {
		STATUSLINE,		// start here. status line is first line of response.
		HEADERS,		// reading in header lines
		BODY,			// waiting for some body data (all or a chunk)
		CHUNKLEN,		// expecting a chunk length indicator (in hex)
		CHUNKEND,		// got the chunk, now expecting a trailing blank line
		TRAILERS,		// reading trailers after body.
		COMPLETE,		// response is complete!
	};
	
class HappyHttpResponse {
	m_Connection = null; 	// to access callback ptrs
	m_Method = null; 		// req method: "GET", "POST" etc...
	m_Url = null; 			// req url: /image.php?d=2...
	m_VersionString = null;	// HTTP-Version
	m_Version = null;		// 10: HTTP/1.0    11: HTTP/1.x (where x>=1)
	m_Status = null;		// Status-Code
	m_Reason = null;		// Reason-Phrase
	m_Headers = null;		// header/value pairs
	m_BytesRead = null;		// body bytes read so far
	m_Chunked = null;		// response is chunked?
	m_ChunkLeft = null;		// bytes left in current chunk
	m_Length = null;		// -1 if unknown
	m_WillClose = null;		// connection will close at response end?
	m_LineBuf = null;		// line accumulation for states that want it
	m_HeaderAccum = null;	// accumulation buffer for headers
	m_State = null;
	
	// only Connection creates Responses.
	constructor(method, url, conn){
		m_Method = method;
		m_Url = url;
		m_Connection = conn;
		m_Version = 0;
		m_Status = 0;
		m_BytesRead = 0;
		m_Chunked = false;
		m_ChunkLeft = 0;
		m_Length = -1;
		m_WillClose = false;
		m_Headers = {};
		m_HeaderAccum = "";
		m_LineBuf = "";
		m_State = Response_state.STATUSLINE;
	}
	
	// retrieve a header (returns null if not present)
	function getheader( name ){
		local lname = name.tolower();
		return m_Headers.get(lname, null);
	}
	
	function completed() { return m_State == Response_state.COMPLETE; }
	
	// get the HTTP status code
	function getstatus(){
		// only valid once we've got the statusline
		assert( m_State != Response_state.STATUSLINE );
		return m_Status;
	}
	
	// get the HTTP response reason string
	function getreason(){
		// only valid once we've got the statusline
		assert( m_State != Response_state.STATUSLINE );
		return m_Reason;
	}
	
	// true if connection is expected to close after this response.
	function willclose() { return m_WillClose; }
	
	function getconnection(){return m_Connection;}
	function geturl() { return m_Url; }
	function getLength() { return m_Length;}
	
	// pump some data in for processing.
	// Returns the number of bytes used.
	// Will always return 0 when response is complete.
	function pump( data, start, datasize ){
		assert( datasize != 0 );
		local count = datasize;
		local data_idx = 0;
		//print(data);
		while( count > 0 && m_State != Response_state.COMPLETE )
		{
			if( m_State == Response_state.STATUSLINE ||
				m_State == Response_state.HEADERS ||
				m_State == Response_state.TRAILERS ||
				m_State == Response_state.CHUNKLEN ||
				m_State == Response_state.CHUNKEND )
			{
				// we want to accumulate a line
				local pos = data.find("\n", data_idx);
				if( pos >= 0 )
				{
					count -= pos-data_idx+1;
					local new_pos = pos;
					if(pos > 0 && data[pos-1] == '\r') --new_pos;
					m_LineBuf = data.slice(data_idx, new_pos);
					data_idx = pos+1;
					// now got a whole line!
					switch( m_State )
					{
						case Response_state.STATUSLINE:
							ProcessStatusLine( m_LineBuf );
							break;
						case Response_state.HEADERS:
							ProcessHeaderLine( m_LineBuf );
							break;
						case Response_state.TRAILERS:
							ProcessTrailerLine( m_LineBuf );
							break;
						case Response_state.CHUNKLEN:
							ProcessChunkLenLine( m_LineBuf );
							break;
						case Response_state.CHUNKEND:
							// just soak up the crlf after body and go to next state
							assert( m_Chunked == true );
							m_State = Response_state.CHUNKLEN;
							break;
						default:
							break;
					}
					m_LineBuf = "";
				}
			}
			else if( m_State == Response_state.BODY )
			{
				local bytesused = 0;
				if( m_Chunked )
					bytesused = ProcessDataChunked( data, data_idx, count );
				else
					bytesused = ProcessDataNonChunked( data, data_idx, count );
				data += bytesused;
				count -= bytesused;
			}
		}
		// return number of bytes used
		return datasize - count;
	}
	
	// tell response that connection has closed
	function notifyconnectionclosed(){
		if( m_State == Response_state.COMPLETE ) return;

		// eof can be valid...
		if( m_State == Response_state.BODY && 
			!m_Chunked && m_Length <= 0 ) Finish();	// we're all done!
		else throw ( "Connection closed unexpectedly" );
	}
	
	function FlushHeader(){
		if( m_HeaderAccum.empty() ) return;	// no flushing required
		local rc = m_HeaderAccum.match("([^:]+):%s*(.+)");
		if(!rc) throw(format("Invalid header (%s)", m_HeaderAccum));
		m_Headers[ rc[0].tolower() ] <- rc[1];
		m_HeaderAccum = "";
	}

	function ProcessStatusLine( line ){
		//HTTP/1.1 200 OK
		local rc = line.match("%s*(%S+)%s+(%d+)%s+(.+)");
		if(!rc) throw(format("BadStatusLine (%s)", line));
		m_VersionString = rc[0];
		m_Status = rc[1].tointeger();
		m_Reason = rc[2];

		if( m_Status < 100 || m_Status > 999 ) throw ( format("BadStatusLine (%s)", line) );

		if( m_VersionString == "HTTP:/1.0" ) m_Version = 10;
		else if( m_VersionString.startswith( "HTTP/1." ) ) m_Version = 11;
		else throw ( format("UnknownProtocol (%s)", m_VersionString) );
		// TODO: support for HTTP/0.9

		// OK, now we expect headers!
		m_State = Response_state.HEADERS;
		m_HeaderAccum = "";		
	}
	
	function ProcessHeaderLine( line ){
		if( line.empty() )
		{
			FlushHeader();
			// end of headers

			// HTTP code 100 handling (we ignore 'em)
			if( m_Status == HTTP_status_code.CONTINUE )
				m_State = Response_state.STATUSLINE;	// reset parsing, expect new status line
			else
				BeginBody();			// start on body now!
			return;
		}

		if( line[0] == ' ' )
		{
			// it's a continuation line - just add it to previous data
			m_HeaderAccum += line;
		}
		else
		{
			// begin a new header
			FlushHeader();
			m_HeaderAccum = line;
		}
	}
	
	function ProcessTrailerLine(line){
		// TODO: handle trailers?
		// (python httplib doesn't seem to!)
		if( line.empty() ) Finish();
		// just ignore all the trailers...
	}
	
	function ProcessChunkLenLine( line ){
		// chunklen in hex at beginning of line
		m_ChunkLeft = line.tointeger(16);

		if( m_ChunkLeft == 0 )
		{
			// got the whole body, now check for trailing headers
			m_State = Response_state.TRAILERS;
			m_HeaderAccum = "";
		}
		else
		{
			m_State = Response_state.BODY;
		}
	}
	
	function ProcessDataChunked( data, data_idx, count ){
		assert( m_Chunked );

		local n = count;
		if( n>m_ChunkLeft ) n = m_ChunkLeft;

		// invoke callback to pass out the data
		m_Connection.response_data( this, data, data_idx, n );

		m_BytesRead += n;

		m_ChunkLeft -= n;
		assert( m_ChunkLeft >= 0);
		if( m_ChunkLeft == 0 )
		{
			// chunk completed! now soak up the trailing CRLF before next chunk
			m_State = Response_state.CHUNKEND;
		}
		return n;
	}
	
	function ProcessDataNonChunked( data, data_idx, count ){
		local n = count;
		if( m_Length != -1 )
		{
			// we know how many bytes to expect
			local remaining = m_Length - m_BytesRead;
			if( n > remaining ) n = remaining;
		}

		// invoke callback to pass out the data
		m_Connection.response_data( this, data, data_idx, n );
		m_BytesRead += n;

		// Finish if we know we're done. Else we're waiting for connection close.
		if( m_Length != -1 && m_BytesRead == m_Length ) Finish();
		return n;
	}
	
	// OK, we've now got all the headers read in, so we're ready to start
	// on the body. But we need to see what info we can glean from the headers
	// first...
	function BeginBody(){
		m_Chunked = false;
		m_Length = -1;	// unknown
		m_WillClose = false;

		// using chunked encoding?
		local trenc = getheader( "transfer-encoding" );
		if( trenc && trenc == "chunked" )
		{
			m_Chunked = true;
			m_ChunkLeft = -1;	// unknown
		}

		m_WillClose = CheckClose();

		// length supplied?
		local contentlen = getheader( "content-length" );
		if( contentlen && !m_Chunked )
		{
			m_Length = contentlen.tointeger();
		}

		// check for various cases where we expect zero-length body
		if( m_Status == HTTP_status_code.NO_CONTENT ||
			m_Status == HTTP_status_code.NOT_MODIFIED ||
			( m_Status >= 100 && m_Status < 200 ) ||		// 1xx codes have no body
			m_Method == "HEAD" )
		{
			m_Length = 0;
		}


		// if we're not using chunked mode, and no length has been specified,
		// assume connection will close at end.
		if( !m_WillClose && !m_Chunked && m_Length == -1 ) m_WillClose = true;

		// Invoke the user callback, if any
		m_Connection.response_begin( this );

	/*
		printf("---------BeginBody()--------\n");
		printf("Length: %d\n", m_Length );
		printf("WillClose: %d\n", (int)m_WillClose );
		printf("Chunked: %d\n", (int)m_Chunked );
		printf("ChunkLeft: %d\n", (int)m_ChunkLeft );
		printf("----------------------------\n");
	*/
		// now start reading body data!
		if( m_Chunked ) m_State = Response_state.CHUNKLEN;
		else m_State = Response_state.BODY;
	}
	
	function CheckClose(){
		if( m_Version == 11 )
		{
			// HTTP1.1
			// the connection stays open unless "connection: close" is specified.
			local conn = getheader( "connection" );
			if( conn && conn == "close" ) return true;
			else return false;
		}

		// Older HTTP
		// keep-alive header indicates persistant connection
		if( getheader( "keep-alive" ) ) return false;

		// TODO: some special case handling for Akamai and netscape maybe?
		// (see _check_close() in python httplib.py for details)
		return true;
	}
	
	function Finish(){
		m_State = Response_state.COMPLETE;
		
		// invoke the callback
		m_Connection.response_complete( this );
	}
}

/*
class MyHappyHttpConnection extends HappyHttpConnection {
	count = null;
	
	constructor(host, port){
		base.constructor(host, port);
		count = 0;
	}
	function response_begin( r ) {
		printf( "BEGIN (%d %s)\n", r->getstatus(), r->getreason() );
		count = 0;
	}
	function response_data( r, data, data_idx, numbytes ) {
		print( data );
		count += numbytes;
	}
	function response_complete( r ) {
		printf( "COMPLETE (%d bytes)\n", count );
	}
}

function test_happyhttp(){
	local start = os.getmillicount();
	local conn = MyHappyHttpConnection( "www.scumways.com", 80);
	conn.request( "GET", "/happyhttp/test.php" );
	while( conn.outstanding() ) conn.pump();
	print(format("Took : %d ms", os.getmillispan(start)));
}

test_happyhttp();
*/
