/*
chmdeco -- extract files from ITS/CHM files and decompile CHM files
Copyright 2003-2007 Paul Wise

This file is part of chmdeco; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place, Suite 330, Boston, MA, 02111-1307, USA or visit:
http://www.gnu.org
*/

#ifndef _COMMON_H_
#define _COMMON_H_

/*
This file contains configuration stuff
We're using a hybrid configuration system
where the best guesses are used when
autoconf variables are not available
This is useful to allow compilation
with non-unix compilers eg MSVC
*/

#ifdef HAVE_CONFIG_H

#include "config.h"

#include <stdio.h> /* everyone has this! */

#ifdef HAVE_SYS_TYPES_H
	#include <sys/types.h>
#endif

#ifdef HAVE_CTYPE_H
	#include <ctype.h>
#endif

#ifdef HAVE_LIMITS_H
	#include <limits.h>
#endif

#ifdef HAVE_STDLIB_H
	#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
	#include <string.h>
#endif

#ifdef HAVE_STRINGS_H
	#include <strings.h>
#endif

#ifdef HAVE_SYS_STAT_H
	#include <sys/stat.h>
#endif

#ifdef HAVE_ERRNO_H
	#include <errno.h>
#endif

#if TM_WITH_SYS_TIME
	#include <sys/time.h>
	#include <time.h>
#else
	#if HAVE_SYS_TIME_H
		#include <sys/time.h>
	#endif
	#include <time.h>
#endif

#ifdef HAVE_UTIME_H
	#include <utime.h>
#endif

#if HAVE_DIRENT_H
	#include <dirent.h>
#else
	#define dirent direct
	#if HAVE_SYS_NDIR_H
		#include <sys/ndir.h>
	#endif
	#if HAVE_SYS_DIR_H
		#include <sys/dir.h>
	#endif
	#if HAVE_NDIR_H
		#include <ndir.h>
	#endif
#endif

#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif

#if !STDC_HEADERS
	#if !HAVE_STRCHR
		#define strchr index
		#define strrchr rindex
	#endif
	#if !HAVE_STRCASECMP
		#define strcasecmp strcmpi
	#endif
#endif

#if !HAVE_MEMCPY
	#define memcpy(d,s,n) bcopy((s),(d),(n)
#endif

#if HAVE_STDBOOL_H
	#include <stdbool.h>
#else
	#if ! HAVE__BOOL
		#ifdef __cplusplus
			typedef bool _Bool;
		#else
			typedef unsigned char _Bool;
		#endif
	#endif
	#define bool _Bool
	#define false 0
	#define true 1
	#define __bool_true_false_are_defined 1
#endif


#else /* !HAVE_CONFIG_H */


#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <utime.h>
#include <dirent.h>
#include <stdbool.h>
#define PACKAGE "chmdeco"
#define VERSION "x.x"

#endif /* !HAVE_CONFIG_H */


/* FIXME: Use autoconf macros for this */
#if defined(__MINGW32__) || defined(_MSC_VER)
/*	#define MKDIR_NEEDS_PERMS 0 */
	#define UINT_DEFINED 0
#else
/*	#define MKDIR_NEEDS_PERMS 1 */
	#define UINT_DEFINED 1
#endif

#if !UINT_DEFINED
	typedef unsigned int uint;
#endif


/* FIXME: Use the macro from the autoconf-archive for this */
/* #if ?MKDIR_NEEDS_PERMS?
	#define mkdir(a,b) mkdir(a,b)
#else */
#if defined(__MINGW32__) || defined(_MSC_VER)
	#define mkdir(a,b) mkdir(a)
#endif

/*
FIXME: Use an autoconf macro for this
For Windows/DOS
*/
#ifndef F_OK
	#define F_OK 0
#endif


/* Some convenience stuff */

#define FREE(a) free(a); (a) = NULL
#define FCLOSE(a) if(a){ fclose(a); (a) = NULL; } /* some systems don't like nulls*/

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef uint8_t BYTE; /* 1-byte integer */
typedef uint16_t WORD; /* 2-byte integer */
typedef uint32_t DWORD; /* 4-byte integer */
typedef uint64_t QWORD; /* 8-byte integer */

typedef QWORD ENCINT; /* Encoded integer base type. This can be increased in size if need be */
typedef QWORD SRINT; /* Encoded integer base type. This can be increased in size if need be */
typedef DWORD LCID;
typedef QWORD FILETIME;

bool exists(char* fname);
FILE* recreate(char* fname);


inline WORD get_WORD(BYTE* b);
inline DWORD get_DWORD(BYTE* b);
inline QWORD get_QWORD(BYTE* b);
bool read_WORD( FILE* f, WORD* w );
bool read_DWORD( FILE* f, DWORD* d );
bool read_QWORD( FILE* f, QWORD* q );

ENCINT get_be_ENCINT( BYTE** b );
SRINT get_SRINT( BYTE** byte, int* bit, BYTE s, BYTE r );


#define AUTHOR "Paul Wise"
#define AUTHOR_EMAIL "<pabs@zip.to>"
#define WEBSITE "http://pabs.zip.to"
#define EMAIL_CHM fprintf( stderr, "%s", "Please email "AUTHOR" "AUTHOR_EMAIL" about this chm.\n" )

#endif /* _COMMON_H_ */
