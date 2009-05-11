INCLUDES = -I$(top_srcdir)/include

noinst_PROGRAMS = ring task_iface
task_iface_SOURCES = task_iface.c
task_iface_LDADD = ../src/libhost.la ../spu/src/*.eo.o

ring_SOURCES = ring_test.c
ring_LDADD = ../shared/libshared.la
