# Process this file with autoconf to produce a configure script.
AC_INIT([chmdeco.c])
AM_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([chmdeco], [0.3])

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_STDC
AC_EXEEXT
AC_PROG_INSTALL
AC_PROG_AWK
AC_CHECK_TOOL([STRIP],[strip])
# FIXME: check for upx and petite for nsi compression

# Checks for libraries.

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h errno.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_STAT
AC_FUNC_STRFTIME
AC_CHECK_FUNCS([atexit getcwd memchr memset mkdir pathconf strcspn strdup strerror strncasecmp strrchr memcpy])

AC_OUTPUT([Makefile chmdeco.spec])
