INCLUDES = -Iinclude

libspeincludedir = $(includedir)/../spu-elf/include/speutils

libspeinclude_HEADERS = include/speutils/spu_message.h \
			include/speutils/messages.h

libincludedir = $(includedir)/speutils

libinclude_HEADERS = include/speutils/messages.h \
			include/speutils/spemessage.h \
			include/speutils/spethread.h

lib_LTLIBRARIES = libspeutils.la
libspeutils_la_SOURCES = spemessage.c spemessage.h spethread.c spethread.h
libspeutils_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

pkgconfigdir = ${libdir}/pkgconfig
pkgconfig_DATA = libspeutils.pc

EXTRA_DIST = COPYING INSTALL
