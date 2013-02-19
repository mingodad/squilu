// FSocket was adapted from K.A. Knizhnik's very nice SAL library.

#if defined(__svr4__)
#define mutex system_mutex
#endif 

#if defined(__FreeBSD__) || defined(__linux__) || defined(__CYGWIN__)
#include <sys/ioctl.h>
#else
#include <stropts.h>
#endif
#include <fcntl.h>
#include <sys/time.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#ifndef __CYGWIN__
#include <netinet/tcp.h>
#endif
#include <unistd.h>
#include <errno.h>
extern "C" {
#include <netdb.h>
}
#undef mutex

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "FSocket_Posix.H"

#include <signal.h>

#define MAX_HOST_NAME     256

char* FSocket_Posix::unix_socket_dir = "/tmp/";

class FSocket_Posix_library { 
 public: 
  FSocket_Posix_library() { 
    static struct sigaction sigpipe_ignore; 
    sigpipe_ignore.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sigpipe_ignore, NULL);
  }
};

static FSocket_Posix_library unisock_lib;

int FSocket_Posix::open(int listen_queue_size) {
  char hostname[MAX_HOST_NAME];
  unsigned short port;
  char* p;
  
  assert(address != NULL);
  
  if ((p = strchr(address, ':')) == NULL 
      || unsigned(p - address) >= sizeof(hostname) 
      || sscanf(p+1, "%hd", &port) != 1)  {
    errcode = bad_address;
    return 0;
  }
  memcpy(hostname, address, p - address);
  hostname[p - address] = '\0';
  
  create_file = 0; 

  union { 
    sockaddr    sock;
    sockaddr_in sock_inet;
    //char        name[MAX_HOST_NAME];
  } u;
  int sa_length;
  
  if (domain == sock_local_domain) { 
    u.sock.sa_family = AF_UNIX;
    
    assert(strlen(unix_socket_dir) + strlen(address) 
	   < MAX_HOST_NAME - offsetof(sockaddr,sa_data)); 
    
    sa_length = offsetof(sockaddr,sa_data) + 
      sprintf(u.sock.sa_data,"%s%s", unix_socket_dir, address);
    
    unlink(u.sock.sa_data); // remove file if existed
    create_file = 1; 
  } else {
    u.sock_inet.sin_family= AF_INET;
    u.sock_inet.sin_addr.s_addr = htonl(INADDR_ANY);
    u.sock_inet.sin_port = htons(port);
    sa_length = sizeof(sockaddr_in);	
  } 
  if ((fd = socket(u.sock.sa_family, SOCK_STREAM, 0)) < 0) { 
    errcode = errno;
    return 0;
  }
  if (bind(fd, &u.sock, sa_length) < 0) {
    errcode = errno;
    ::close(fd);
    return 0;
  }
  if (listen(fd, listen_queue_size) < 0) {
    errcode = errno;
    ::close(fd);
    return 0;
  }
  errcode = ok;
  state = ss_open;
  return 1;
}

int  FSocket_Posix::valid() {
  return errcode == ok;
}

void FSocket_Posix::get_error_text(char* buf, size_t buf_size) {
  char* msg; 
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
    msg = strerror(errcode);
  }
  strncpy(buf, msg, buf_size);
}


FSocket* FSocket_Posix::accept() {
  int s;
  
  if (state != ss_open) { 
    errcode = not_opened;
    return NULL;
  }
  
  while((s = ::accept(fd, NULL, NULL )) < 0 && errno == EINTR);
  
  if (s < 0) { 
    errcode = errno;
    return NULL;
  } else if (state != ss_open) {
    errcode = not_opened;
    return NULL;
  } else { 
    static struct linger l = {1, LINGER_TIME};
    if (domain == sock_global_domain) { 
      int enabled = 1;
      if (setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*)&enabled, 
		     sizeof enabled) != 0) {
	errcode = errno;
	::close(s);	
	return NULL;
      }
    }
    if (setsockopt(s, SOL_SOCKET, SO_LINGER, (char*)&l, sizeof l) != 0) { 
      errcode = invalid_access_mode; 
      ::close(s);
      return NULL; 
    }
    errcode = ok;
    return new FSocket_Posix(s); 
  }
}

int FSocket_Posix::cancel_accept()  {
  // Wakeup listener
  state = ss_shutdown;
  delete FSocket::connect(address, domain, 1, 0);
  return 1;
}    


int FSocket_Posix::connect(int max_attempts, time_t timeout) {
  int   rc;
  char* p;
  char hostname[MAX_HOST_NAME];
  unsigned short port;
  
  assert(address != NULL);
  
  if (domain != sock_local_domain) { 
    if ((p = strchr(address, ':')) == NULL 
	|| unsigned(p - address) >= sizeof(hostname) 
	|| sscanf(p+1, "%hd", &port) != 1) {
      errcode = bad_address;
      return 0;
    }
    memcpy(hostname, address, p - address);
    hostname[p - address] = '\0';
  }
  create_file = 0; 
  
  union { 
    sockaddr    sock;
    sockaddr_in sock_inet;
    char        name[MAX_HOST_NAME];
  } u;
  
  int sa_length;
  
  if (domain == sock_local_domain || (domain == sock_any_domain && 
				      strcmp(hostname, "localhost") == 0)) {
    // connect UNIX socket
    u.sock.sa_family = AF_UNIX;
    
    assert(strlen(unix_socket_dir) + strlen(address) 
	   < MAX_HOST_NAME - offsetof(sockaddr,sa_data)); 
    
    sa_length = offsetof(sockaddr,sa_data) +
      sprintf(u.sock.sa_data, "%s%s", unix_socket_dir, address);
  } else { 
    u.sock_inet.sin_family = AF_INET;  
    u.sock_inet.sin_addr.s_addr = inet_addr(hostname);
    
    if ((int)(u.sock_inet.sin_addr.s_addr) == -1) {
      struct hostent* hp;  // entry in hosts table
      if ((hp=gethostbyname(hostname)) == NULL || 
	  hp->h_addrtype != AF_INET) {
	errcode = bad_address;
	return 0;
      }
      memcpy(&u.sock_inet.sin_addr,hp->h_addr,sizeof u.sock_inet.sin_addr);
    }
    u.sock_inet.sin_port = htons(port);
    sa_length = sizeof(u.sock_inet);
  }
  while (1) {
    if ((fd = socket(u.sock.sa_family, SOCK_STREAM, 0)) < 0) { 
      errcode = errno;
      return 0;
    }
    do { 
      rc = ::connect(fd, &u.sock, sa_length);
    } while (rc < 0 && errno == EINTR);
    
    if (rc < 0) { 
      errcode = errno;
      ::close(fd);
      if (errcode == ENOENT || errcode == ECONNREFUSED) {
	if (--max_attempts > 0) { 
	  sleep(timeout);
	} else { 
	  break;
	}
      } else {
	return 0;
      }
    } else {
      if (u.sock_inet.sin_family == AF_INET) { 
	int enabled = 1;
	if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&enabled, 
		       sizeof enabled) != 0)
	  {
	    errcode = errno;
	    ::close(fd);	
		    return 0;
	  }
      }
      errcode = ok;
      state = ss_open;
      return 1;
    }
  }
  errcode = connection_failed;
  return 0;
}

int FSocket_Posix::read(void* buf, size_t min_size, size_t max_size, 
		      time_t timeout) { 
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
    ssize_t rc; 
    if (timeout != WAIT_FOREVER) { 
      fd_set events;
      struct timeval tm;
      FD_ZERO(&events);
      FD_SET(fd, &events);
      tm.tv_sec = timeout;
      tm.tv_usec = 0;
      while ((rc = select(fd+1, &events, NULL, NULL, &tm)) < 0 
	     && errno == EINTR);
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
    while ((rc = ::read(fd, (char*)buf + size, max_size - size)) < 0 
	   && errno == EINTR); 
    if (rc < 0) { 
      errcode = errno;
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
	

int FSocket_Posix::read(void* buf, size_t size) { 
  if (state != ss_open) { 
    errcode = not_opened;
    return 0;
  }
  
  do { 
    ssize_t rc; 
    while ((rc = ::read(fd, buf, size)) < 0 && errno == EINTR); 
    if (rc < 0) { 
      errcode = errno;
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


int FSocket_Posix::write(void const* buf, size_t size) { 
  if (state != ss_open) { 
    errcode = not_opened;
    return 0;
  }
  
  do { 
    ssize_t rc; 
    while ((rc = ::write(fd, buf, size)) < 0 && errno == EINTR); 
    if (rc < 0) { 
      errcode = errno;
      return 0;
    } else if (rc == 0) {
      errcode = broken_pipe;
      return 0; 
    } else { 
      buf = (char*)buf + rc; 
      size -= rc; 
    }
  } while (size != 0); 
  
  //
  // errcode is not assigned 'ok' value beacuse write function 
  // can be called in parallel with other socket operations, so
  // we want to preserve old error code here.
  //
  return 1;
}
	
int FSocket_Posix::close() {
  if (state != ss_close) {
    state = ss_close;
    if (::close(fd) == 0) {
      errcode = ok;
      return 1;
    } else { 
      errcode = errno;
      return 0;
    }
  }
  errcode = ok;
  return 1;
}

int FSocket_Posix::shutdown() {
  if (state == ss_open) { 
    state = ss_shutdown;
    int rc = ::shutdown(fd, 2);
    if (rc != 0) { 
      errcode = errno;
      return 0;
    } 
  } 
  return 1;
}

FSocket_Posix::~FSocket_Posix() {
  close();
  if (create_file) { 
    char name[MAX_HOST_NAME];
    sprintf(name, "%s%s", unix_socket_dir, address);
    unlink(name);
  }
  delete[] address; 
}

FSocket_Posix::FSocket_Posix(const char* addr, socket_domain domain) { 
  address = strdup(addr); 
  this->domain = domain;
  create_file = 0;
  errcode = ok;
}

FSocket_Posix::FSocket_Posix(int new_fd) { 
  fd = new_fd; 
  address = NULL; 
  create_file = 0;
  state = ss_open; 
  errcode = ok;
}

FSocket* FSocket::create_local(char const* address, int listen_queue_size) {
  FSocket_Posix* sock = new FSocket_Posix(address, sock_local_domain);
  sock->open(listen_queue_size); 
  return sock;
}

FSocket* FSocket::create_global(char const* address, int listen_queue_size) {
  FSocket_Posix* sock = new FSocket_Posix(address, sock_global_domain);
  sock->open(listen_queue_size); 
  return sock;
}

FSocket* FSocket::connect(char const* address, 
			    socket_domain domain, 
			    int max_attempts, 
			    time_t timeout) {
  FSocket_Posix* sock = new FSocket_Posix(address, domain);
  sock->connect(max_attempts, timeout); 
  return sock;
}
    
char const* get_process_name() { 
  static char name[MAX_HOST_NAME+8];
  struct utsname local_host;
  uname(&local_host);
  sprintf(name, "%s:%d", local_host.nodename, (int)getpid());
  return name;
}
