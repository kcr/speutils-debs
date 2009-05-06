INCLUDES = -I$(top_srcdir)/../include -I$(top_srcdir)/include

noinst_PROGRAMS = spu_taskmanager

spu_taskmanager_SOURCES = spu_taskmanager.c spu_taskpipe.c

noinst_DATA = spu_taskmanager.eo

%.eo: %
	$(EMBEDSPU) $<_handle $< $@
# ugly workaround to make libtool happy
	cp $@ $@.o


_SOURCES = spu_taskpipe.h
