// FSocket was adapted from K.A. Knizhnik's very nice SAL library.

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "FSocket_Win32.H"

#define MAX_HOST_NAME         256
#define MILLISECOND           1000

static HANDLE WatchDogMutex;

class win_socket_library { 
  public:
    win_socket_library() { 
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(1, 1), &wsa) != 0) {
	    fprintf(stderr,"Failed to initialize windows sockets: %d\n",
		    WSAGetLastError());
	}
	//
	// This mutex is used to recognize process termination
	//
	WatchDogMutex = CreateMutex(NULL, TRUE, NULL);
    }
    ~win_socket_library() {
	WSACleanup();
    }
};

static win_socket_library ws32_lib;

int FSocket_Win32::open(int listen_queue_size)
{
    unsigned short port;
    char* p;

    assert(address != NULL);

    if ((p = strchr(address, ':')) == NULL 
	|| sscanf(p+1, "%hd", &port) != 1) 
    {
	errcode = bad_address;
	return 0;
    }
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) { 
	errcode = WSAGetLastError();
	return 0;
    }
    struct sockaddr_in insock;
    insock.sin_family = AF_INET;
    insock.sin_addr.s_addr = htonl(INADDR_ANY);
    insock.sin_port = htons(port);
    
    if (bind(s, (sockaddr*)&insock, sizeof(insock)) != 0) { 
	errcode = WSAGetLastError();
	closesocket(s);
	return 0;
    }
    if (listen(s, listen_queue_size) != 0) {
	errcode = WSAGetLastError();
	closesocket(s);
	return 0;
    } 
    errcode = ok;
    state = ss_open;
    return 1;
}

int FSocket_Win32::valid()
{
    return errcode == ok;
}

void FSocket_Win32::get_error_text(char* buf, size_t buf_size)
{
    int   len;
    char* msg; 
    char  msgbuf[64];

    switch(errcode) { 
      case ok:
        msg = "ok";
	break;
      case not_opened:
	msg = "socket not opened";
        break;
      case bad_address: 
	msg = "bad address";
	break;
      case connection_failed: 
	msg = "exceed limit of attempts of connection to server";
	break;
      case broken_pipe:
	msg = "connection is broken";
	break; 
      case invalid_access_mode:
        msg = "invalid access mode";
	break;
      default: 
	len = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
		      NULL,
		      errcode,
		      0,
		      buf,
		      buf_size,
		      NULL);
	if (len == 0) { 
            sprintf(msgbuf, "unknown error code %u", errcode);
	    msg = msgbuf;
	}
    }
    strncpy(buf, msg, buf_size);
}

FSocket* FSocket_Win32::accept()
{
    if (state != ss_open) { 
	errcode = not_opened;
	return NULL;
    }

    SOCKET new_sock = ::accept(s, NULL, NULL );

    if (new_sock == INVALID_SOCKET) { 
	errcode = WSAGetLastError();
	return NULL;
    } else { 
	static struct linger l = {1, LINGER_TIME};
	if (setsockopt(s, SOL_SOCKET, SO_LINGER, (char*)&l, sizeof l) != 0) { 
	    errcode = invalid_access_mode; 
	    closesocket(s);
	    return NULL; 
	}
	int enabled = 1;
	if (setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*)&enabled, 
		       sizeof enabled) != 0)
	{
	    errcode = WSAGetLastError();
	    closesocket(s);	
	    return NULL;
	}
	errcode = ok;
	return new FSocket_Win32(new_sock); 
    }
}

int FSocket_Win32::cancel_accept() 
{
    int result = close();
    // Wakeup listener
    delete FSocket::connect(address, sock_global_domain, 1, 0);
    return result;
}    


int FSocket_Win32::connect(int max_attempts, time_t timeout)
{
    char hostname[MAX_HOST_NAME];
    char *p;
    unsigned short port;

    assert(address != NULL);

    if ((p = strchr(address, ':')) == NULL 
	|| p - address >= sizeof(hostname) 
	|| sscanf(p+1, "%hd", &port) != 1) 
    {
	errcode = bad_address;
	return 0;
    }
    memcpy(hostname, address, p - address);
    hostname[p - address] = '\0';

    struct sockaddr_in insock;  // inet socket address
    struct hostent*    hp;      // entry in hosts table
	
    if ((hp = gethostbyname(hostname)) == NULL || hp->h_addrtype != AF_INET) {
	errcode = bad_address;
	return 0;
    }
    insock.sin_family = AF_INET;
    insock.sin_port = htons(port);
    memcpy(&insock.sin_addr, hp->h_addr, sizeof insock.sin_addr);
    
    while (1) {
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) { 
	    errcode = WSAGetLastError();
	    return 0;
	}
	if (::connect(s, (sockaddr*)&insock, sizeof insock) != 0) { 
	    errcode = WSAGetLastError();
	    closesocket(s);
	    if (errcode == WSAECONNREFUSED) {
		if (--max_attempts > 0) {  
		    Sleep(timeout*MILLISECOND);
		} else { 
		    break;
		}
            } else {
		return 0;
	    }
	} else {
	    int enabled = 1;
	    if (setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*)&enabled, 
			   sizeof enabled) != 0)
	    {
		errcode = WSAGetLastError();
		closesocket(s);	
		return 0;
	    }
	    errcode = ok;
	    state = ss_open;
	    return 1;
	}
    }
    errcode = connection_failed;
    return 0;
}

int FSocket_Win32::read(void* buf, size_t min_size, size_t max_size, 
		     time_t timeout)
{ 
    size_t size = 0;
    time_t start = 0;
    if (state != ss_open) { 
	errcode = not_opened;
	return -1;
    }
    if (timeout != WAIT_FOREVER) { 
	start = time(NULL); 
    }

    do { 
	int rc;
	if (timeout != WAIT_FOREVER) { 
	    fd_set events;
	    struct timeval tm;
	    FD_ZERO(&events);
	    FD_SET(s, &events);
	    tm.tv_sec = timeout;
	    tm.tv_usec = 0;
	    rc = select(s+1, &events, NULL, NULL, &tm);
	    if (rc < 0) { 
		errcode = errno;
		return -1;
	    }
	    if (rc == 0) {
		return size;
	    }
	    time_t now = time(NULL);
	    timeout = start + timeout >= now ? 0 : timeout + start - now;  
	}
	rc = recv(s, (char*)buf + size, max_size - size, 0);
	if (rc < 0) { 
	    errcode = WSAGetLastError();
	    return -1;
	} else if (rc == 0) {
	    errcode = broken_pipe;
	    return -1; 
	} else { 
	    size += rc; 
	}
    } while (size < min_size); 

    return (int)size;
}

int FSocket_Win32::read(void* buf, size_t size)
{ 
    if (state != ss_open) { 
	errcode = not_opened;
	return 0;
    }

    do { 
	int rc = recv(s, (char*)buf, size, 0);
	if (rc < 0) { 
	    errcode = WSAGetLastError();
	    return 0;
	} else if (rc == 0) {
	    errcode = broken_pipe;
	    return 0; 
	} else { 
	    buf = (char*)buf + rc; 
	    size -= rc; 
	}
    } while (size != 0); 

    return 1;
}
	

int FSocket_Win32::write(void const* buf, size_t size)
{ 
    if (state != ss_open) { 
	errcode = not_opened;
	return 0;
    }
    
    do { 
	int rc = send(s, (char*)buf, size, 0);
	if (rc < 0) { 
	    errcode = WSAGetLastError();
	    return 0;
	} else if (rc == 0) {
	    errcode = broken_pipe;
	    return 0; 
	} else { 
	    buf = (char*)buf + rc; 
	    size -= rc; 
	}
    } while (size != 0); 

    return 1;
}
	
int FSocket_Win32::shutdown()
{
    if (state == ss_open) { 
	state = ss_shutdown;
        int rc = ::shutdown(s, 2);
        if (rc != 0) {
	    errcode = WSAGetLastError();
	    return 0;
	} 
    } 
    errcode = ok;
    return 1;
}


int FSocket_Win32::close()
{
    if (state != ss_close) { 
	state = ss_close;
	if (closesocket(s) == 0) { 
	    errcode = ok;
	    return 1;
	} else { 
	    errcode = WSAGetLastError();
	    return 0;
	}
    }
    return 1;
}

FSocket_Win32::~FSocket_Win32()
{
    close();
}

FSocket_Win32::FSocket_Win32(const char* addr)
{ 
    address = strdup(addr); 
    errcode = ok;
    s = INVALID_SOCKET;
}

FSocket_Win32::FSocket_Win32(SOCKET new_sock) 
{ 
    s = new_sock; 
    address = NULL; 
    state = ss_open;
    errcode = ok;
}

FSocket* FSocket::create_local(char const* address, int listen_queue_size)
{
    FSocket_Win32_Local* sock = new FSocket_Win32_Local(address);
    sock->open(listen_queue_size);
    return sock;
}

FSocket* FSocket::create_global(char const* address, int listen_queue_size)
{
    FSocket_Win32* sock = new FSocket_Win32(address);
    sock->open(listen_queue_size); 
    return sock;
}

FSocket* FSocket::connect(char const* address, 
			    socket_domain domain, 
			    int max_attempts,
			    time_t timeout)
{
    size_t hostname_len;
    char*  port;
    
    if (domain == sock_local_domain 
	|| (domain == sock_any_domain 
	    && ((port = strchr(address, ':')) == NULL 
		|| ((hostname_len = port - address) == 9 
		    && strncmp(address, "localhost", hostname_len) == 0))))
     {
        FSocket_Win32_Local* s = new FSocket_Win32_Local(address);
        s->connect(max_attempts, timeout); 
	return s;
     } else { 
        FSocket_Win32* s = new FSocket_Win32(address);
        s->connect(max_attempts, timeout); 
        return s;
    }  
}
    
char const* get_process_name() 
{ 
    static char name[MAX_HOST_NAME+8];
    gethostname(name, MAX_HOST_NAME); 
    sprintf(name + strlen(name), ":%x", GetCurrentProcessId());
    return name;
}

//
// Local windows sockets
//

int FSocket_Win32_Local::read(void* buf, size_t min_size, size_t max_size, 
			   time_t timeout)
{
    time_t start = 0;
    char* dst = (char*)buf;
    size_t size = 0;
    Error = ok;
    if (timeout != WAIT_FOREVER) { 
	start = time(NULL); 
	timeout *= 1000; // convert seconds to miliseconds
    }
    while (size < min_size && state == ss_open) {	
        RcvBuf->RcvWaitFlag = 1;
	size_t begin = RcvBuf->DataBeg;
	size_t end = RcvBuf->DataEnd;
	size_t rcv_size = (begin <= end)
	    ? end - begin : sizeof(RcvBuf->Data) - begin;
	if (rcv_size > 0) { 
	    RcvBuf->RcvWaitFlag = 0;
            if (rcv_size >= max_size) { 
	        memcpy(dst, &RcvBuf->Data[begin], max_size);
		begin += max_size;
		size += max_size;
	    } else { 
	        memcpy(dst, &RcvBuf->Data[begin], rcv_size);
		begin += rcv_size;
		dst += rcv_size;
		size += rcv_size;
	    } 
	    RcvBuf->DataBeg = (begin == sizeof(RcvBuf->Data)) ? 0 : begin;
	    if (RcvBuf->SndWaitFlag) { 
		SetEvent(Signal[RTR]);
    	    }		
	} else {
	    HANDLE h[2];
	    h[0] = Signal[RD];
	    h[1] = Mutex;
	    int rc = WaitForMultipleObjects(2, h, false, timeout);
	    RcvBuf->RcvWaitFlag = false;
	    if (rc != WAIT_OBJECT_0) {
		if (rc == WAIT_OBJECT_0+1 || rc == WAIT_ABANDONED+1) { 
		    Error = broken_pipe;
		    ReleaseMutex(Mutex);
		} else if (rc == WAIT_TIMEOUT) { 
		    return size;
		} else { 
		    Error = GetLastError();
		}
		return -1;
	    }
	    if (timeout != WAIT_FOREVER) { 
		time_t now = time(NULL);
		timeout = timeout >= (now - start)*1000 
		    ? 0 : timeout - (now - start)*1000;  
	    }
	}
    }			
    return size < min_size ? -1 : (int)size;
}

int FSocket_Win32_Local::read(void* buf, size_t size)
{
    char* dst = (char*)buf;
    Error = ok;
    while (size > 0 && state == ss_open) {	
        RcvBuf->RcvWaitFlag = 1;
	size_t begin = RcvBuf->DataBeg;
	size_t end = RcvBuf->DataEnd;
	size_t rcv_size = (begin <= end)
	    ? end - begin : sizeof(RcvBuf->Data) - begin;
	if (rcv_size > 0) { 
	    RcvBuf->RcvWaitFlag = 0;
            if (rcv_size >= size) { 
	        memcpy(dst, &RcvBuf->Data[begin], size);
		begin += size;
		size = 0;
	    } else { 
	        memcpy(dst, &RcvBuf->Data[begin], rcv_size);
		begin += rcv_size;
		dst += rcv_size;
		size -= rcv_size;
	    } 
	    RcvBuf->DataBeg = (begin == sizeof(RcvBuf->Data)) ? 0 : begin;
	    if (RcvBuf->SndWaitFlag) { 
		SetEvent(Signal[RTR]);
    	    }		
	} else {
	    HANDLE h[2];
	    h[0] = Signal[RD];
	    h[1] = Mutex;
	    int rc = WaitForMultipleObjects(2, h, FALSE, INFINITE);
	    RcvBuf->RcvWaitFlag = 0;
	    if (rc != WAIT_OBJECT_0) {
		if (rc == WAIT_OBJECT_0+1 || rc == WAIT_ABANDONED+1) { 
		    Error = broken_pipe;
		    ReleaseMutex(Mutex);
		} else { 
		    Error = GetLastError();
		}
		return 0;
	    }
	}
    }			
    return size == 0;
}

int FSocket_Win32_Local::write(const void* buf, size_t size)
{
    char* src = (char*)buf;
    Error = ok;
    while (size > 0 && state == ss_open) {	
        SndBuf->SndWaitFlag = 1;
	size_t begin = SndBuf->DataBeg;
	size_t end = SndBuf->DataEnd;
	size_t snd_size = (begin <= end) 
	    ? sizeof(SndBuf->Data) - end - (begin == 0)
	    : begin - end - 1;
	if (snd_size > 0) { 
	    SndBuf->SndWaitFlag = 0;
            if (snd_size >= size) { 
	        memcpy(&SndBuf->Data[end], src, size);
		end += size;
	        size = 0;
	    } else { 
	        memcpy(&SndBuf->Data[end], src, snd_size);
		end += snd_size;
		src += snd_size;
		size -= snd_size;
	    } 
	    SndBuf->DataEnd = (end == sizeof(SndBuf->Data)) ? 0 : end;
	    if (SndBuf->RcvWaitFlag) { 
		SetEvent(Signal[TD]);
    	    }		
	} else {
	    HANDLE h[2];
	    h[0] = Signal[RTT];
	    h[1] = Mutex;
	    int rc = WaitForMultipleObjects(2, h, FALSE, INFINITE);
	    RcvBuf->SndWaitFlag = 0;
	    if (rc != WAIT_OBJECT_0) {
		if (rc == WAIT_OBJECT_0+1 || rc == WAIT_ABANDONED+1) { 
		    Error = broken_pipe;
		    ReleaseMutex(Mutex);
		} else { 
		    Error = GetLastError();
		}	
		return 0;
	    }
	}
    }				
    return size == 0;
}

#define MAX_ADDRESS_LEN 64

FSocket_Win32_Local::FSocket_Win32_Local(const char* address)
{
    Name = strdup(address);
    Error = not_opened;
    Mutex = NULL;
}
 
int FSocket_Win32_Local::open(int)
{
    char buf[MAX_ADDRESS_LEN];	
    int  i;

    for (i = RD; i <= RTT; i++) {  
        sprintf(buf, "%s.%c", Name, i + '0');
	Signal[i] = CreateEvent(NULL, 0, 0, buf);
	if (GetLastError() == ERROR_ALREADY_EXISTS) { 
	    WaitForSingleObject(Signal[i], 0);
	}
	if (!Signal[i]) {
	    Error = GetLastError();
	    while (--i >= 0) { 
		CloseHandle(Signal[i]);
            }
	    return 0;
        }	
    }
    sprintf(buf, "%s.shr", Name);
    BufHnd = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
                               0, sizeof(socket_buf)*2, buf);
    if (!BufHnd) {
	Error = GetLastError();
	for (i = RD; i <= RTT; i++) {  
	    CloseHandle(Signal[i]);
        }
	return 0;
    }
    RcvBuf = (socket_buf*)MapViewOfFile(BufHnd, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (!RcvBuf) {
	Error = GetLastError();
	CloseHandle(BufHnd);
	for (i = RD; i <= RTT; i++) {  
	    CloseHandle(Signal[i]);
        }
	return 0;
    }	
    SndBuf = RcvBuf+1;
    RcvBuf->DataBeg = RcvBuf->DataEnd = 0;
    SndBuf->DataBeg = SndBuf->DataEnd = 0;	 
    Error = ok;
    state = ss_open;
    return 1;
}

FSocket_Win32_Local::FSocket_Win32_Local()
{
    int i;
    BufHnd = NULL;
    Mutex = NULL; 
    Name = NULL;

    for (i = RD; i <= RTT; i++) {  
	Signal[i] = CreateEvent(NULL, 0, 0, NULL);
	if (!Signal[i]) {
	    Error = GetLastError();
	    while (--i >= 0) { 
		CloseHandle(Signal[i]);
            }
	    return;
        }	
    }
    // create anonymous shared memory section
    BufHnd = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
                               0, sizeof(socket_buf)*2, NULL);
    if (!BufHnd) {
	Error = GetLastError();
	for (i = RD; i <= RTT; i++) {  
	    CloseHandle(Signal[i]);
        }
	return;
    }
    RcvBuf = (socket_buf*)MapViewOfFile(BufHnd, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (!RcvBuf) {
	Error = GetLastError();
	CloseHandle(BufHnd);
	for (i = RD; i <= RTT; i++) {  
	    CloseHandle(Signal[i]);
        }
	BufHnd = NULL;
	return;
    }	
    SndBuf = RcvBuf+1;
    RcvBuf->DataBeg = RcvBuf->DataEnd = 0;
    SndBuf->DataBeg = SndBuf->DataEnd = 0;	 
    Error = ok;
    state = ss_open;
}

FSocket_Win32_Local::~FSocket_Win32_Local()
{
    close();
    delete[] Name;
}	

FSocket* FSocket_Win32_Local::accept()
{   
    HANDLE h[2];

    if (state != ss_open) {	
	return NULL;
    }
		    
    connect_data* cdp = (connect_data*)SndBuf->Data;
    cdp->Pid = GetCurrentProcessId();
    cdp->Mutex = WatchDogMutex;
    while (1) { 
	SetEvent(Signal[RTR]);
	int rc = WaitForSingleObject(Signal[RD], ACCEPT_TIMEOUT);
	if (rc == WAIT_OBJECT_0) {
	    if (state != ss_open) { 
		Error = not_opened;
		return NULL;
	    }
	    Error = ok;
	    break;
	} else if (rc != WAIT_TIMEOUT) { 
	    Error = GetLastError();
	    return NULL;
	}
    }
    FSocket_Win32_Local* sock = new FSocket_Win32_Local();
    sock->Mutex = ((connect_data*)RcvBuf->Data)->Mutex;
    accept_data* adp = (accept_data*)SndBuf->Data;
    adp->BufHnd = sock->BufHnd;
    for (int i = RD; i <= RTT; i++) { 
	adp->Signal[(i + TD - RD) & RTT] = sock->Signal[i]; 
    }
    SetEvent(Signal[TD]);
    h[0] = Signal[RD];
    h[1] = sock->Mutex;
    int rc = WaitForMultipleObjects(2, h, FALSE, INFINITE);
    if (rc != WAIT_OBJECT_0) {
	if (rc == WAIT_OBJECT_0+1 || rc == WAIT_ABANDONED+1) { 
	    Error = broken_pipe;
	    ReleaseMutex(Mutex);
	} else { 
	    Error = GetLastError();
	}	
	delete sock;
	return NULL;
    }    
    return sock;
}

int FSocket_Win32_Local::cancel_accept() 
{
    state = ss_shutdown;
    SetEvent(Signal[RD]);
    SetEvent(Signal[RTT]);
    return 1;
}    

int FSocket_Win32_Local::valid()
{
    return !Error;
}

int FSocket_Win32_Local::close()
{
    if (state != ss_close) { 		
	state = ss_close;
	if (Mutex) { 
	    CloseHandle(Mutex);
	}
	for (int i = RD; i <= RTT; i++) { 
	    CloseHandle(Signal[i]);
        }
	UnmapViewOfFile(RcvBuf < SndBuf ? RcvBuf : SndBuf);
        CloseHandle(BufHnd);	
	Error = not_opened;
    }
    return 1;
}

void FSocket_Win32_Local::get_error_text(char* buf, size_t buf_size)
{
    switch (Error) { 
      case ok:
        strncpy(buf, "ok", buf_size);
	break;
      case not_opened:
        strncpy(buf, "socket not opened", buf_size);
	break;
      case broken_pipe:
        strncpy(buf, "connection is broken", buf_size);
	break;
      case timeout_expired:
        strncpy(buf, "connection timeout expired", buf_size);
	break;
      default: 	
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
		      NULL,
		      Error,
		      0,
		      buf,
		      buf_size,
		      NULL);
    }
}


int FSocket_Win32_Local::shutdown()
{
    if (state == ss_open) { 
    	state = ss_shutdown;
	SetEvent(Signal[RD]); 	
	SetEvent(Signal[RTT]); 	
    }
    return 1;
}

int FSocket_Win32_Local::connect(int max_attempts, time_t timeout)
{
    char buf[MAX_ADDRESS_LEN];
    int  rc, i, error_code;
    HANDLE h[2];

    for (i = RD; i <= RTT; i++) {  
        sprintf(buf, "%s.%c", Name, ((i + TD - RD) & RTT) + '0');
	Signal[i] = CreateEvent(NULL, 0, 0, buf);
	if (!Signal[i]) {
	    Error = GetLastError();
	    while (--i >= 0) { 
		CloseHandle(Signal[i]);
            }
	    return 0;
        }	
    }
    sprintf(buf, "%s.shr", Name);
    BufHnd = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
                               0, sizeof(socket_buf)*2, buf);
    if (!BufHnd) {
	Error = GetLastError();
	for (i = RD; i <= RTT; i++) {  
	    CloseHandle(Signal[i]);
        }
	return 0;
    }
    SndBuf = (socket_buf*)MapViewOfFile(BufHnd, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (!SndBuf) { 
	Error = GetLastError();
	for (i = RD; i <= RTT; i++) {  
	    CloseHandle(Signal[i]);
        }
	CloseHandle(BufHnd);
	return 0;
    }
    RcvBuf = SndBuf+1;
    state = ss_shutdown;
    Mutex = NULL;

    rc = WaitForSingleObject(Signal[RTT],timeout*max_attempts*MILLISECOND);
    if (rc != WAIT_OBJECT_0) {
	error_code = rc == WAIT_TIMEOUT ? timeout_expired : GetLastError();
	close();
	Error = error_code;
	return 0;
    }
    connect_data* cdp = (connect_data*)RcvBuf->Data;
    HANDLE hServer = OpenProcess(STANDARD_RIGHTS_REQUIRED|PROCESS_DUP_HANDLE,
				 FALSE, cdp->Pid);
    if (!hServer) { 
	error_code = GetLastError();
	close();
	Error = error_code;
	return 0;
    }
    HANDLE hSelf = GetCurrentProcess();
    if (!DuplicateHandle(hServer, cdp->Mutex, hSelf, &Mutex, 
			 0, FALSE, DUPLICATE_SAME_ACCESS) ||
	!DuplicateHandle(hSelf, WatchDogMutex, hServer, 
			 &((connect_data*)SndBuf->Data)->Mutex, 
			 0, FALSE, DUPLICATE_SAME_ACCESS))
    {
        error_code = GetLastError();
	CloseHandle(hServer);
	close();
	Error = error_code;
	return 0;
    }
    SetEvent(Signal[TD]);
    h[0] = Signal[RD];
    h[1] = Mutex;
    rc = WaitForMultipleObjects(2, h, FALSE, INFINITE);

    if (rc != WAIT_OBJECT_0) { 
	if (rc == WAIT_OBJECT_0+1 || rc == WAIT_ABANDONED+1) { 
	    error_code = broken_pipe;
	    ReleaseMutex(Mutex);
	} else { 
	    error_code = GetLastError();
	}
	CloseHandle(hServer);
	close();
	Error = error_code;
	return 0;
    }
    accept_data ad = *(accept_data*)RcvBuf->Data;

    SetEvent(Signal[TD]);
    for (i = RD; i <= RTT; i++) { 
	CloseHandle(Signal[i]);
    }
    UnmapViewOfFile(SndBuf);
    CloseHandle(BufHnd);	
    BufHnd = NULL;

    if (!DuplicateHandle(hServer, ad.BufHnd, hSelf, &BufHnd, 
			 0, FALSE, DUPLICATE_SAME_ACCESS))
    {
	Error = GetLastError();
	CloseHandle(hServer);
	CloseHandle(Mutex); 
	return 0;
    } else { 
	for (i = RD; i <= RTT; i++) { 
	    if (!DuplicateHandle(hServer, ad.Signal[i], 
				 hSelf, &Signal[i], 
				 0, FALSE, DUPLICATE_SAME_ACCESS))
	    {
		Error = GetLastError();
		CloseHandle(hServer);
		CloseHandle(BufHnd); 
		CloseHandle(Mutex); 
		while (--i >= 0) CloseHandle(Signal[1]);
		return 0;
	    }
	}
    }
    CloseHandle(hServer);

    SndBuf = (socket_buf*)MapViewOfFile(BufHnd, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (!SndBuf) { 
	Error = GetLastError();
	CloseHandle(BufHnd); 
	CloseHandle(Mutex); 
	for (i = RD; i <= RTT; i++) {  
	    CloseHandle(Signal[i]);
        }
	return 0;
    }
    RcvBuf = SndBuf+1;
    Error = ok;
    state = ss_open; 
    return 1;
}

