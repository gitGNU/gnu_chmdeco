#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>

/* Some convenience stuff */

#define FREE(a) free(a); (a) = NULL
#define FCLOSE(a) if(a){ fclose(a); (a) = NULL; } /* linux don't like nulls*/

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef uint8_t BYTE; /* 1-byte integer */
typedef uint16_t WORD; /* 2-byte integer */
typedef uint32_t DWORD; /* 4-byte integer */
typedef uint64_t QWORD; /* 8-byte integer */

typedef char bool;
#define true 1
#define false 0

typedef DWORD ENCINT; /* Encoded integer base type. This can be increased in size if need be to support huge files */
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

#include "version.h"

#define AUTHOR "Pabs"
#define AUTHOR_EMAIL "<pabs3@zip.to>"
#define WEBSITE "http://zip.to/pabs3/"
#define EMAIL_CHM "Please email "AUTHOR" "AUTHOR_EMAIL" about this chm.\n"

#if defined(__MINGW32__) || defined(_MSC_VER)
	#define MKDIR_PERM
	typedef unsigned int uint;
#else
	#define MKDIR_PERM , 0666
#endif

#endif /* _COMMON_H_ */
