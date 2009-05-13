AC_INIT(spu-medialib/spe, VERSION)

if test -d "/usr/spu-elf"; then
	host_alias=spu-elf
	CC=spu-elf-gcc
	#AC_PROG_CC(spu-elf-gcc)

else
	host_alias=spu
	CC=spu-gcc
	#AC_PROG_CC(spu-gcc)

fi

AC_SUBST(host_alias)
#AC_PROG_CXX(spu-elf-g++)
m4_undefine([AC_PROG_CXX])
m4_defun([AC_PROG_CXX],[])
m4_undefine([AC_PROG_F77])
m4_defun([AC_PROG_F77],[])
AC_PROG_LIBTOOL
AM_INIT_AUTOMAKE(dist-bzip2 no-dist-gzip foreign)
AM_MAINTAINER_MODE
  
AC_CONFIG_MACRO_DIR([m4])

AC_PATH_TOOL(EMBEDSPU, embedspu)

AC_SUBST(DESTDIR)

AC_CONFIG_FILES([Makefile
		src/Makefile])

AC_OUTPUT
