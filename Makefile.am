SUBDIRS = src spu tst


libincludedir = $(includedir)/speutils

libinclude_HEADERS = include/speutils/messages.h \
		include/speutils/spemessage.h \
		include/speutils/spethread.h\
		include/speutils/spefifo.h

lib_LTLIBRARIES = libspeutils.la
libspeutils_la_LIBADD = src/libhost.la spu/src/*.eo.o
libspeutils_la_SOURCES =
libspeutils_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

pkgconfigdir = ${libdir}/pkgconfig
pkgconfig_DATA = libspeutils.pc

EXTRA_DIST = COPYING INSTALL
