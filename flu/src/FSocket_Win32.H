/* -*-C++-*-

   "$Id: FSocket_Win32.H,v 1.2 2000/05/16 16:58:12 jamespalmer Exp $"

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

#ifndef __FSOCKET_WIN32_H__
#define __FSOCKET_WIN32_H__

#include <FLU/FSocket.H>

class FSocket_Win32 : public FSocket {
 protected:
  SOCKET s;
  int errcode;  // error code of last failed operation
  char* address;  // host address

  enum error_codes {
    ok = 0,
    not_opened = -1,
    bad_address = -2,
    connection_failed = -3,
    broken_pipe = -4,
    invalid_access_mode = -5
  };

 public:
  int open(int listen_queue_size);
  int connect(int max_attempts, time_t timeout);

  int read(void* buf, size_t min_size, size_t max_size,time_t timeout);
  int read(void* buf, size_t size);
  int write(void const* buf, size_t size);

  int valid();
  int close();
  int shutdown();
  void get_error_text(char* buf, size_t buf_size);

  FSocket* accept();
  int cancel_accept();

  FSocket_Win32(const char* address);
  FSocket_Win32(SOCKET new_sock);

  ~FSocket_Win32();
};

#define SOCKET_BUF_SIZE (8*1024)
#define ACCEPT_TIMEOUT  (30*1000)

class FSocket_Win32_Local : public FSocket {
 protected:
  enum error_codes {
    ok = 0,
    not_opened = -1,
    broken_pipe = -2,
    timeout_expired = -3
  };
  enum socket_signals {
    RD,  // receive data
    RTR, // ready to receive
    TD,  // transfer data
    RTT  // ready to transfer
  };
  //------------------------------------------------------
  // Mapping between signals at opposite ends of socket:
  // TD  ---> RD
  // RTR ---> RTT
  //------------------------------------------------------

  struct socket_buf {
    volatile int RcvWaitFlag;
    volatile int SndWaitFlag;
    volatile int DataEnd;
    volatile int DataBeg;
    char Data[SOCKET_BUF_SIZE - 4*sizeof(int)];
  };
  struct accept_data {
    HANDLE Signal[4];
    HANDLE BufHnd;
  };
  struct connect_data {
    HANDLE Mutex;
    int    Pid;
  };
  socket_buf* RcvBuf;
  socket_buf* SndBuf;
  HANDLE      Signal[4];
  HANDLE      Mutex;
  HANDLE      BufHnd;
  int         Error;
  char*       Name;

 public:
  int open(int listen_queue_size);
  int connect(int max_attempts, time_t timeout);

  int read(void* buf, size_t min_size, size_t max_size,time_t timeout);
  int read(void* buf, size_t size);
  int write(void const* buf, size_t size);

  int valid();
  int close();
  int shutdown();
  void get_error_text(char* buf, size_t buf_size);

  FSocket* accept();
  int cancel_accept();

  FSocket_Win32_Local(const char* address);
  FSocket_Win32_Local();

  ~FSocket_Win32_Local();
};

#endif

