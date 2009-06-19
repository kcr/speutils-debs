INCLUDES = -I$(top_srcdir)/../include -I$(top_srcdir)/../shared \
	-I$(top_srcdir)/include


noinst_PROGRAMS = spu_taskmanager

spu_taskmanager_SOURCES = ../../shared/linkedlist.c spu_taskmanager.c \
	spu_taskpipe.c

noinst_DATA = spu_taskmanager.eo

%.eo: %
	$(EMBEDSPU) $<_handle $< $@
# ugly workaround to make libtool happy
	cp $@ $@.o


_SOURCES = spu_taskpipe.h
bin_PROGRAMS = pic_test
pic_test_SOURCES = pic_test.c
pic_test_LDFLAGS = -e 0x80

AM_CFLAGS = -fpic

