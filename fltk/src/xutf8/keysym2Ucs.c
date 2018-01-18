/* "$Id: keysym2Ucs.c 12379 2017-08-14 11:42:34Z AlbrechtS $"
 *
 * Author: Jean-Marc Lienher ( http://oksid.ch )
 * Copyright 2000-2003 by O'ksi'D.
 *
 * This library is free software. Distribution and use rights are outlined in
 * the file "COPYING" which should have been included with this file.  If this
 * file is missing or damaged, see the license at:
 *
 *     http://www.fltk.org/COPYING.php
 *
 * Please report all bugs and problems on the following page:
 *
 *     http://www.fltk.org/str.php
 */

#include "../Xutf8.h"
#include "imKStoUCS.c"

long XKeysymToUcs(KeySym keysym) {
  return (long) KeySymToUcs4(keysym);
}

/*
 * End of "$Id: keysym2Ucs.c 12379 2017-08-14 11:42:34Z AlbrechtS $".
 */
