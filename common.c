/*
chmdeco -- extract files from ITS/CHM files and decompile CHM files
Copyright (C) 2003 Pabs

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



/*
common.c - this module implements some convenience functions
It was written by Pabs.
*/



/* Local headers */

#include "common.h"



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

/* read a big endian ENCINT from memory */
ENCINT get_be_ENCINT( BYTE** b ){
    ENCINT ret = 0;
    int shift=0;

    do {
        ret |= ((**b) & 0x7f) << shift;
        shift += 7;
    } while (*((*b)++) & 0x80);

    return ret;
}

/*
find the first unset bit in memory
return the number of set bits found
return -1 if the buffer runs out before we find an unset bit
*/
int ffus(BYTE** byte, int* bit/* , uint len */){
	int bits = 0;
	while( **byte & ( 1 << *bit ) ){
		/* if( !len-- ) return -1; */
		if(*bit) (*bit)--;
		else { (*byte)++; *bit = 7; }
		bits++;
	}
	if(*bit) (*bit)--;
	else { (*byte)++; *bit = 7; }
	return bits;
}

/* #ifndef log2
	#define log2(a) (log(a)/log(2))
#endif */

/* read a scale & root encoded integer from memory */
/* Does not yet support s =4, 8, 16, etc */
SRINT get_SRINT( BYTE** byte, int* bit, BYTE s, BYTE r ){
	SRINT ret; BYTE mask;
	int n, n_bits, num_bits, base, count;
	if( !byte || !*byte || !bit || *bit > 7 || s != 2 )
		return ~(SRINT)0;
	ret = 0;
	count = ffus( byte, bit );
	n_bits = n = r + /* log2(s) * */ (count ? count-1 : 0) ;
	while( n > 0 ){
		num_bits = n > *bit ? *bit : n-1;
		base = n > *bit ? 0 : *bit - (n-1);
		switch( num_bits ){
			case 0: mask = 1; break;
			case 1: mask = 3; break;
			case 2: mask = 7; break;
			case 3: mask = 0xf; break;
			case 4: mask = 0x1f; break;
			case 5: mask = 0x3f; break;
			case 6: mask = 0x7f; break;
			case 7: mask = 0xff; break;
                        default:
                                mask = 0xff;
                                fputs( "chmdeco: warning: impossible condition found reading s/r int\n", stderr );
                        break;
		}
		mask <<= base;
		ret = ( ret << (num_bits+1) ) | (SRINT)((**byte & mask) >> base);
		if( n > *bit ){
			(*byte)++;
			n -= *bit+1;
			*bit = 7;
		} else {
			*bit -= n;
			n = 0;
		}
	}
	if( count ) ret |= (SRINT)1 << n_bits;
	return ret;
}
