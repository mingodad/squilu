/* -*-C++-*-

   "$Id: FSocket_Posix.H,v 1.2 2000/05/16 16:58:12 jamespalmer Exp $"

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

#ifndef __FSOCKET_POSIX_H__
#define __FSOCKET_POSIX_H__

#include <FLU/FSocket.H>

class FSocket_Posix : public FSocket {
 protected:
  descriptor_t fd;
  int errcode; // error code of last failed operation
  char* address; // host address
  socket_domain domain; // Unix domain or INET socket
  int create_file; // Unix domain sockets use files for connection

  enum error_codes {
    ok = 0,
    not_opened = -1,
    bad_address = -2,
    connection_failed = -3,
    broken_pipe = -4,
    invalid_access_mode = -5
  };

 public:
  //
  // Directory for Unix Domain socket files. This directory should be
  // either empty or be terminated with "/". Dafault value is "/tmp/"
  //
  static char* unix_socket_dir;

  int open(int listen_queue_size);
  int connect(int max_attempts, time_t timeout);

  int read(void* buf, size_t min_size, size_t max_size,time_t timeout);
  int read(void* buf, size_t size);
  int write(void const* buf, size_t size);

  int valid();
  int shutdown();
  int close();
  void get_error_text(char* buf, size_t buf_size);

  FSocket* accept();
  int cancel_accept();

  FSocket_Posix(const char* address, socket_domain domain);
  FSocket_Posix(int new_fd);

  ~FSocket_Posix();
};

#endif
