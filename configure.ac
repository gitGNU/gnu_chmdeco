# chmdeco -- extract files from ITS/CHM files and decompile CHM files
# Copyright (C) 2003 Pabs

# This file is part of chmdeco; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place, Suite 330, Boston, MA, 02111-1307, USA or visit:
# http://www.gnu.org

# Process this file with autoconf to produce a configure script.
AC_INIT([chmdeco.c])
AM_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([chmdeco], [0.3.1])

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
AC_HEADER_STDBOOL
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
