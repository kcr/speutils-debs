SUBDIRS = src spu


libincludedir = $(includedir)/speutils

libinclude_HEADERS = include/speutils/messages.h \
			include/speutils/spemessage.h \
			include/speutils/spethread.h

lib_LTLIBRARIES = libspeutils.la
libspeutils_la_LIBADD = src/libspemessage.la src/libspethread.la
libspeutils_la_SOURCES =
libspeutils_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

pkgconfigdir = ${libdir}/pkgconfig
pkgconfig_DATA = libspeutils.pc

EXTRA_DIST = COPYING INSTALL
