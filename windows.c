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
windows.c - this module implements reading the #WINDOWS file
It was written by Pabs.
*/



/* Local headers */

#include "chmdeco.h"
#include "common.h"
#include "windows.h"
#include "strings_file.h"



FILE* windows = NULL;

DWORD num_windows = 0;
DWORD entry_size = 0;
BYTE* entry_buf = NULL;

bool open_windows(){

	if(windows) return true;

	errno = 0;
	windows = fopen("#WINDOWS","rb");
	if(windows){

		if( read_DWORD(windows,&num_windows) && num_windows && read_DWORD(windows,&entry_size) ){
			if( entry_size < 0x10 ){
				fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "#WINDOWS", "entries are too small for this decompiler" );
			} else if( (entry_buf = (BYTE*)malloc(entry_size)) ){
				atexit(close_windows);
				return true;
			} else
				fprintf( stderr, "%s: %s %s: %s\n", PACKAGE, input, "#WINDOWS buffer", strerror(errno) );
		} else if( errno )
			fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "#WINDOWS", strerror(errno) );

		FCLOSE(windows);

	} else if( errno != ENOENT )
		fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "#WINDOWS", strerror(errno) );

	return false;
}

#define NAV_PANE_STYLE       0x00000002
#define STYLE_FLAGS          0x00000004
#define EXT_STYLE_FLAGS      0x00000008
#define INIT_POS             0x00000010
#define NAV_PANE_WIDTH       0x00000020
#define SHOW_STATE           0x00000040
#define INFO_TYPES           0x00000080
#define BUTTONS              0x00000100
#define NAV_PANE_INIT_CLOSED 0x00000200
#define TAB_POS              0x00000400
#define TAB_ORDER            0x00000800
#define HIST_COUNT           0x00001000
#define DEF_PANE             0x00002000

/* FIXME: This could be more robust eg read the size DWORD first & trust it */
bool print_windows_entry(FILE* hhp){
	if( fread(entry_buf,entry_size,1,windows) ){

		char* s;
		DWORD valid_members, o, min_en_s = entry_size-4;

		print_string( hhp, get_DWORD(entry_buf+8) );
		fputs( "=", hhp );
		valid_members = get_DWORD(entry_buf+0xC);
#define PRINT(a) \
		if( (a) <= min_en_s && (o = get_DWORD(entry_buf+(a))) && o != 0xFFFFFFFF ){ \
			if( (s = get_string(o)) ){ \
				fprintf( hhp, "\"%s\"", s ); \
				FREE(s); \
			} \
		} \
		fputs( ",", hhp );
		PRINT(0x14)
		PRINT(0x60)
		PRINT(0x64)
		PRINT(0x68)
		PRINT(0x6C)
		PRINT(0xA4)
		PRINT(0x9C)
		PRINT(0xA8)
		PRINT(0xA0)
#undef PRINT
#define PRINT(V,P,O) \
		if( (O) <= min_en_s && (valid_members & (V)) ) fprintf( hhp, (P), get_DWORD(entry_buf+(O)) ); \
		fputs( ",", hhp );
		PRINT(NAV_PANE_STYLE,"0x%x",0x10)
		PRINT(NAV_PANE_WIDTH,"%u",0x4C)
		PRINT(BUTTONS,"0x%x",0x70)
		if( 0x2C <= min_en_s && (valid_members & INIT_POS) )
			fprintf( hhp, "[%u,%u,%u,%u]",
				get_DWORD(entry_buf+0x20),
				get_DWORD(entry_buf+0x24),
				get_DWORD(entry_buf+0x28),
				get_DWORD(entry_buf+0x2C)
			);
		fputs( ",", hhp );
		PRINT(STYLE_FLAGS,"0x%x",0x18)
		PRINT(EXT_STYLE_FLAGS,"0x%x",0x1C)
		PRINT(SHOW_STATE,"%u",0x30)
		PRINT(NAV_PANE_INIT_CLOSED,"%u",0x74)
		PRINT(DEF_PANE,"%u",0x78)
		PRINT(TAB_POS,"%u",0x7C)
#undef PRINT
		fprintf( hhp, "%u\r\n", get_DWORD(entry_buf+0x80) );
		return true;
	} else if( ferror( windows ) )
		fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "#WINDOWS", strerror(errno) );

	return false;
}

void close_windows( void ){
	FREE(entry_buf);
	FCLOSE(windows);
}
