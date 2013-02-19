/* -*-C++-*- 

   "$Id: FSocket.H,v 1.3 2000/05/16 16:58:12 jamespalmer Exp $"
   
   Copyright 1997 GARRET.
   Copyright 1999-2000 by the Flek development team.
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA.
   
   Please report all bugs and problems to "flek-devel@sourceforge.net".

*/

// FSocket was adapted from K.A. Knizhnik's very nice SAL library.

#ifndef __FSOCKET_H__
#define __FSOCKET_H__

#include <time.h>

#define DEFAULT_CONNECT_MAX_ATTEMPTS 100
#define DEFAULT_RECONNECT_TIMEOUT    1  // seconds
#define DEFAULT_LISTEN_QUEUE_SIZE    5
#define LINGER_TIME                  10 // seconds
#define WAIT_FOREVER                 ((time_t)-1)

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <windows.h>
typedef HANDLE descriptor_t; 
#else
typedef int descriptor_t; 
#endif

/**
 * @package libflek_core
 * FSocket is an abstract socket interface.  
 * FSocket was adapted from K.A. Knizhnik's very nice SAL library.
 * It provides a socket implementation based on the socket library
 * provided by the operating system.  As local sockets are not supported 
 * under Win32, shared memory and semaphore objects are used to provide
 * high speed communications on the same computer.
 */
class FSocket { 
 public: 
  virtual int read(void* buf, size_t min_size, size_t max_size,
		   time_t timeout = WAIT_FOREVER) = 0;
  
  /**
   * Read data from socket.
   * @param buf Buffer to hold fetched data.
   * @param buf_size Number of bytes to fetch.
   * @return 1 if operation successfully completed, 0 otherwise. 
   */
  virtual int read(void* buf, size_t size) = 0;

  /**
   * Write data to socket.
   * @param buf Buffer that contains the data to be sent.
   * @param buf_size Number of bytes to send.
   * @return 1 if operation successfully completed, 0 otherwise.
   */
  virtual int write(void const* buf, size_t size) = 0;
  
  /**
   * Check the status of the last operation with socket. 
   * @return 1 if the last operation completed successfully, 0 otherwise.
   */
  virtual int valid() = 0; 
  
  /**
   * Get error message text for the last operation. 
   * @param buf Buffer to receive text of the error message.
   * @param buf_size Size of buffer, no more than buf_size bytes will be 
   * placed in the buffer.
   */
  virtual void get_error_text(char* buf, size_t buf_size) = 0;

  /**
   * Accept new socket. This method is called by server to establish a
   * connection with the new client. When the client executes a connect 
   * method to the access server's accept port, accept method will create 
   * new socket, which can be used for communication with the client. 
   * The accept method will block the current task until some connection 
   * is established. 
   * @return Pointer to new socket or NULL if operation failed.
   */
  virtual FSocket* accept() = 0;
  
  /**
   * Cancel accept operation. Task blocked in accept call is woken
   * and execution continues.
   * @return 1 if socket was successfully closed, 0 otherwise.
   */
  virtual int cancel_accept() = 0;
  
  /**
   * Shutdown the socket. This function prohibits write and read 
   * operation on the socket. All further attempts to read or write 
   * data from/to the socket will be denied.  But all previously 
   * initiated operations are guaranteed to be completed. 
   * @return 1 if operation successfully completed, 0 otherwise. 
   */
  virtual int shutdown() = 0;
  
  /**
   * Close socket connection.
   * @return 1 if operation successfully completed, 0 otherwise.
   */
  virtual int close() = 0;
  
  enum socket_domain { 
    sock_any_domain,   // domain is chosen automatically
    sock_local_domain, // local domain (i.e. Unix domain socket) 
    sock_global_domain // global domain (i.e. INET sockets) 
  };

  /**
   * Establish connection with server. This method will do at most 
   * max_attempts attempts to connect server, with timeout interval 
   * between attempts.
   * @param address Address of server socket in format "hostname:port".
   * @param domain Type of connection. The following values of this 
   * parameter are recognized:
   * <ul>
   *   <li><b>sock_any_domain</b> = domain is chosen automatically.</li>
   *   <li><b>sock_local_domain</b> = local domain (connection with one host).</li>
   *   <li><b>sock_global_domain</b> = internet domain.</li>
   * </ul>
   * If sock_any_domain is specified, local connection is chosen when 
   * either port was omitted in specification of the address or hostname 
   * is "localhost", and global connection is used in all other cases. 
   * @param max_attempts Maximal number of attempts to connect to server.
   * @param timeout Timeout in seconds between attempts to connect the server.
   * @return This method always create new socket object and returns a 
   * pointer to it. If connection with server was not established, this 
   * socket contains error code describing reason of failure.  So returned 
   * socket should be first checked by the valid() method. 
   */
  static FSocket* connect(char const* address, 
			   socket_domain domain = sock_any_domain, 
			   int max_attempts = DEFAULT_CONNECT_MAX_ATTEMPTS,
			   time_t timeout = DEFAULT_RECONNECT_TIMEOUT);
  
  /**
   * Create and open socket in local domain at the server site. 
   * @param address Address to be assigned to the socket. 
   * @param listen_queue_size Size of listen queue.
   * @return This method always create new socket object and returns 
   * pointer to it. If socket can not be opened, error code field of 
   * returned socket describes the reason of failure. So returned
   * socket should be first checked by the valid() method. 
   */
  static FSocket* create_local(char const* address,
				int listen_queue_size = 
				DEFAULT_LISTEN_QUEUE_SIZE);
  
  /**
   * Create and open socket in global (internet) domain at the server site.
   * @param address Address to be assigned to the socket.
   * @param listen_queue_size Size of listen queue.
   * @return This method always create new socket object and returns 
   * pointer to it. If socket can not be opened, error code field of 
   * returned socket describes the reason of failure. So returned
   * socket should be first checked by the valid() method. 
   */
  static FSocket* create_global(char const* address,
				 int listen_queue_size = 
				 DEFAULT_LISTEN_QUEUE_SIZE);
  
  virtual ~FSocket() {} 
  FSocket() { state = ss_close; }
  
 protected:
  enum { ss_open, ss_shutdown, ss_close } state;
};

// 
// Return current host name + identifier of current process
//
extern char const* get_process_name(); 

#endif

