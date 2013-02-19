#! /bin/sh
#
#  Build static libraries, hiding (some) ickiness from the makefile

ACTION=$1; shift
LIBRARY=$1; shift
VERSION=$1; shift

case "$ACTION" in
make)   /mingw/bin/ar crv $LIBRARY.a "$@"
	/mingw/bin/ranlib $LIBRARY.a
	rm -f $LIBRARY
	/bin/ln $LIBRARY.a $LIBRARY
	;;
files)  echo "${LIBRARY}.a"
	;;
install)/bin/install -m 644 ${LIBRARY}.a $1
	;;
esac
