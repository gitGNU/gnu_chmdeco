/*
chmdeco -- extract files from ITS/CHM files and decompile CHM files
Copyright (C) 2003 Pabs

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place, Suite 330, Boston, MA, 02111-1307, USA or visit:
http://www.gnu.org
*/



/*
common.c - this module implements some convenience functions
It was written by Pabs.
*/



/* System headers */

#include <stdio.h>
#include <unistd.h>



/* Local headers */

#include "common.h"

/* For Windows/DOS */
#ifndef F_OK
	#define F_OK 0
#endif

bool exists(char* fname){
	return !access(fname,F_OK);
}

/* FIXME: don't require dir changes, or at least not chdir("..")? */
/* If only POSIX allowed multiple current directories (one write, multi read)
or pushd/popd as part of the standard C library */

FILE* recreate(char* fname){
	FILE* ret;
	int ch = chdir("#recreated");
	ret = fopen(fname,"wb");
	if(!ch) chdir("..");
	return ret;
}

/* Some functions to facilitate reading lendian integers */

inline WORD get_WORD(BYTE* b){
	return b[0]|b[1]<<8;
}

inline DWORD get_DWORD(BYTE* b){
	return b[0]|b[1]<<8|b[2]<<16|b[3]<<24;
}

inline QWORD get_QWORD(BYTE* b){
	return b[0]|b[1]<<8|b[2]<<16|b[3]<<24|(QWORD)b[4]<<32|(QWORD)b[5]<<40|(QWORD)b[6]<<48|(QWORD)b[7]<<56;
}

bool read_WORD( FILE* f, WORD* w ){
	BYTE b[2];
	if( !fread( b, 2, 1, f) ) return false;
	*w = get_WORD(b);
	return true;
}

bool read_DWORD( FILE* f, DWORD* d ){
	BYTE b[4];
	if( !fread( b, 4, 1, f) ) return false;
	*d = get_DWORD(b);
	return true;
}

bool read_QWORD( FILE* f, QWORD* q ){
	BYTE b[8];
	if( !fread(b, 8, 1, f) ) return false;
	*q = get_QWORD(b);
	return true;
}
