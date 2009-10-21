INCLUDES = -I$(top_srcdir)/include

#noinst_PROGRAMS = linkedlist ring task_iface
noinst_PROGRAMS = ring task_iface
task_iface_SOURCES = task_iface.c
task_iface_LDADD = ../src/libhost.la ../spu/src/*.eo.o

ring_SOURCES = ring_test.c
ring_LDADD = ../shared/libshared.la
#linkedlist_SOURCES = list_test.c
#linkedlist_LDADD = $(top_builddir)/shared/libshared.la
