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
system.c - this module implements an interface to the #SYSTEM file
It was written by Pabs.
*/



/* System headers */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>



/* Local headers */

#include "chmdeco.h"
#include "common.h"
#include "system.h"



FILE* system_file = NULL;
DWORD system_version;
struct system_entry system_entries[MAX_CODE];

bool open_system( void ){

	WORD code, length;

	if( system_file ) return true;

	for( code = 0; code < MAX_CODE; code++){
		system_entries[code].offset = -1;
		system_entries[code].length = 0;
	}

	system_file = fopen( "#SYSTEM", "rb" );
	if( system_file ){

		if( read_DWORD(system_file, &system_version) ){

			for(;;){
				read_WORD(system_file, &code);
				if( ferror(system_file) ) goto ERROR;
				if( feof(system_file) ) break;
				read_WORD(system_file, &length);
				if( ferror(system_file) ) goto ERROR;
				if( code < MAX_CODE ){
					system_entries[code].length = length;
					system_entries[code].offset = ftell(system_file);
				} else
					fprintf( stderr, "%s: warning: %s/%s: %s\n%s", PROGNAME, input, "#SYSTEM", "unknown entry code found", EMAIL_CHM );
				fseek(system_file, length, SEEK_CUR);
				/* FIXME: Cache data? */
			}

			atexit(close_system);
			return true;
		}
	}
ERROR:
	if( errno != ENOENT ) fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "#SYSTEM", strerror(errno) );
	FCLOSE(system_file);
	return false;
}

BYTE* get_system(WORD code){

	if( code >= MAX_CODE ){
		fprintf( stderr, "%s: bug: %s/%s: invalid code accessed (%u)\n", PROGNAME, input, "#SYSTEM", code );
		return NULL;
	}

	if( system_entries[code].offset < 0 ) return NULL;
	else{

		BYTE* ret = (BYTE*)malloc(system_entries[code].length+1);
		if(ret){

			ret[system_entries[code].length]=0;
			fseek(system_file, system_entries[code].offset, SEEK_SET);
			if( !fread( ret, system_entries[code].length, 1, system_file) ){
				fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "#SYSTEM", strerror(errno) );
				FREE(ret);
			}

		} else fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "#SYSTEM buffer", strerror(errno) );
		return ret;

	}
}

bool get_system_DWORD(WORD code, DWORD* d){

	if( code >= MAX_CODE ){
		fprintf( stderr, "%s: bug: %s/%s: invalid code accessed (%u)\n", PROGNAME, input, "#SYSTEM", code );
		return false;
	}

	if( system_entries[code].offset < 0 ) return false;
	else if( system_entries[code].offset >= 4 ) {

		bool ret;
		fseek(system_file, system_entries[code].offset, SEEK_SET);
		ret = read_DWORD( system_file, d );
		if(!ret) fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "#SYSTEM", strerror(errno) );
		return ret;

	} else {
		fprintf( stderr, "%s: bug: %s/%s: %s\n", PROGNAME, input, "#SYSTEM", "entry too small" );
		return false;
	}
}

bool get_system_to(WORD code, BYTE* buf, WORD length){

	if( code >= MAX_CODE ){
		fprintf( stderr, "%s: bug: %s/%s: invalid code accessed (%u)\n", PROGNAME, input, "#SYSTEM", code );
		return false;
	}

	if( system_entries[code].offset < 0 ){
		if( length < system_entries[code].length )
			fprintf( stderr, "%s: bug: %s/%s: %s\n", PROGNAME, input, "#SYSTEM", "not enough storage in buffer" );
		return false;
	} else {
		bool ret;
		fseek(system_file, system_entries[code].offset, SEEK_SET);
		ret = fread( buf, system_entries[code].length, 1, system_file) ? true : false;
		if(!ret) fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "#SYSTEM", strerror(errno) );
		return ret;
	}
}

void close_system( void ){
	FCLOSE(system_file);
}
