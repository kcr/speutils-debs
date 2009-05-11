#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
m4_define([speutils_major_version], [0])
m4_define([speutils_minor_version], [1])
m4_define([speutils_micro_version], [0])
m4_define([speutils_interface_age], [0])
m4_define([speutils_binary_age],
          [m4_eval(speutils_minor_version + speutils_micro_version)])
m4_define([speutils_version],
          [speutils_major_version.speutils_minor_version.speutils_micro_version])
m4_define([speutils_lt_current],
          [m4_eval(speutils_minor_version + speutils_micro_version - speutils_interface_age)])
m4_define([speutils_lt_revision], [speutils_interface_age])
m4_define([speutils_lt_age], [m4_eval(speutils_binary_age - speutils_interface_age)])

AC_PREREQ(2.61)
AC_INIT(speutils, [speutils_version], somewhere)
# Checks for programs.
AM_INIT_AUTOMAKE

m4_undefine([AC_PROG_CXX])
m4_defun([AC_PROG_CXX],[])
m4_undefine([AC_PROG_F77])
m4_defun([AC_PROG_F77],[])

AC_PROG_LIBTOOL
# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_HEADER_TIME
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([gettimeofday sqrt])

AC_CONFIG_FILES([Makefile
		src/Makefile
		tst/Makefile
		shared/Makefile])
# we need libspe2
PKG_CHECK_MODULES(LIBSPE2, [libspe2 >= 2.2.0])
CFLAGS="$CFLAGS $LIBSPE2_CFLAGS"
LIBS="$LIBS $LIBSPE2_LIBS"

LT_CURRENT=speutils_lt_current
LT_REVISION=speutils_lt_revision
LT_AGE=speutils_lt_age

AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)


AC_CONFIG_FILES([libspeutils.pc])
# additional configure for spu stuff
# No spu code to compile yet.. 
AC_CONFIG_SUBDIRS([spu])

AC_OUTPUT()
