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
misc.c - this module implements stp, context sensitivity, samples output.
It was written by Pabs.
*/



/* System headers */

#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



/* Local headers */

#include "chmdeco.h"
#include "strings.h"
#include "misc.h"



bool stp_success = false;
#define STP_SEPARATOR "\r\n"

/* FIXME: search for .stp files too? */
/* FIXME: that many indentations are just fugly */
void recreate_fts_stop_list_file( void ){

	FILE* objinst = fopen("$OBJINST","rb");
	stp_success = false;
	if(objinst){

		DWORD num;

		fseek(objinst,4,SEEK_SET);
		if( read_DWORD(objinst,&num) ){

			DWORD i, offset, length; off_t cur_off = ftell(objinst);
			/* {4662DAAF-D393-11D0-9A56-00C04FB68BF7} */
			BYTE guid[16]; BYTE need[16] = "\xaf\xda\x62\x46\x93\xd3\xd0\x11\x9a\x56\0\xc0\x4f\xb6\x8b\xf7";

			for( i = 0; i < num; i++ ){

				fseek(objinst,cur_off,SEEK_SET);
				if( read_DWORD(objinst,&offset) && read_DWORD(objinst,&length) ){

					cur_off = ftell(objinst);
					fseek(objinst,offset,SEEK_SET);
					if( fread(guid,16,1,objinst) ){
						if( !memcmp(guid,need,16) ){

							DWORD flags;

							fseek(objinst,4,SEEK_CUR);
							if( read_DWORD(objinst,&flags) ){
								if( flags&4 ){

									fseek(objinst,2631+4,SEEK_CUR);
									if( read_DWORD(objinst,&length) ){
										if( length ){

											BYTE* buf = (BYTE*)malloc(length+2);
											if( buf ){

												fseek(objinst,32,SEEK_CUR);
												if( fread(buf,length,1,objinst) ){

													FILE* stp = recreate(FTS_STOP_LIST_FILE_NAME);
													if(stp){

														WORD len;
														char* s = (char*)buf;
														buf[length+1] = buf[length] = 0;

														while( (len = get_WORD(s)) ){
															s+=2; fwrite(s,len,1,stp); s+=len;
															/* FIXME: output the stp 1 word per line, all on one line or a limited number of chars/words per line? */
															fputs( STP_SEPARATOR, stp );
														}

														if( ferror(stp) )
															fprintf( stderr, "%s: %s/%s/%s: %s\n", PROGNAME, input, "#recreated", FTS_STOP_LIST_FILE_NAME, strerror(errno) );
														else stp_success = true;

														FCLOSE(stp);

														/* We're done */
														break;

													} else fprintf( stderr, "%s: %s/%s/%s: %s\n", PROGNAME, input, "#recreated", FTS_STOP_LIST_FILE_NAME, strerror(errno) );
												} else fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "$OBJINST", strerror(errno) );
												FREE(buf);
											} else goto MERROR;
										}
									} else goto FERROR;
								}
							} else goto FERROR;
						}
					} else goto FERROR;
				} else goto FERROR;
			}
		} else goto FERROR;
		FCLOSE(objinst);
	}
	return;

FERROR:
	fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "$OBJINST", strerror(errno) );
	FCLOSE(objinst);
	return;

MERROR:
	fprintf( stderr, "%s: %s %s: %s\n", PROGNAME, input, "stp buffer", strerror(errno) );
	FCLOSE(objinst);
}

bool alias_map_success = false;

void recreate_alias_map( void ){
	FILE* ivb;

	errno = 0;
	alias_map_success = false;
	ivb = fopen("#IVB","rb");
	if( ivb ){

		FILE* alias = recreate(ALIAS_FILE_NAME);
		if(alias){
			FILE* map = recreate(MAP_FILE_NAME);
			if(map){

				DWORD tmp;

				fseek( ivb, 4, SEEK_SET );

				for(;;){

					if( read_DWORD( ivb, &tmp ) ){

						fprintf( map, "#define IDH_%u %u\r\n", tmp, tmp );
						fprintf( alias, "IDH_%u=", tmp );

						if( read_DWORD( ivb, &tmp ) )
							print_string( alias, tmp );

						fputs( "\r\n", alias );

					} else if( ferror(ivb) ){
						fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "#IVB", strerror(errno) );
						break;
					}

					if( feof(ivb) ) break;
				}

				if( ferror(map) )
					fprintf( stderr, "%s: %s/%s/%s: %s\n", PROGNAME, input, "#recreated", MAP_FILE_NAME, strerror(errno) );
				if( ferror(alias) )
					fprintf( stderr, "%s: %s/%s/%s: %s\n", PROGNAME, input, "#recreated", ALIAS_FILE_NAME, strerror(errno) );

				FCLOSE(map);
			} else fprintf( stderr, "%s: %s/%s/%s: %s\n", PROGNAME, input, "#recreated", MAP_FILE_NAME, strerror(errno) );
			FCLOSE(alias);
		} else fprintf( stderr, "%s: %s/%s/%s: %s\n", PROGNAME, input, "#recreated", ALIAS_FILE_NAME, strerror(errno) );
		FCLOSE(ivb);
	} else if( errno && errno != ENOENT )
		fprintf( stderr, "%s: %s/%s: %s\n", PROGNAME, input, "#IVB", strerror(errno) );

}

bool samples_success = false;
char Sample_Staging_Path[8] = "samples";

void recreate_samples( void ){
	samples_success = false;
	fprintf( stderr, "%s: warning: %s: %s\n", PROGNAME, input, "samples decompilation not yet implemented" );
	/*
	DIR* dir = opendir( "." );
	if( dir ){
		struct stat st;
		struct dirent* de;
		while( (de = readdir( dir )) ){
			if( !strcasecmp( de->d_name, "samples" ){
				if( !stat( de->d_name, &st ) && S_ISDIR( st.st_mode ) ){
					FIXME: if a folder named samples has any *.sfl in it and a html file has a sample obj in it
						then write those files out to the hhs
					strcpy( sample_staging_path, de->d_name );
					chdir( sample_staging_path );
					if( find( "*.sfl" ) && grep( "*.html", Samples_Object) ){
						write out crap to sample-list-file.hhs
					}
				}
			}
		}
		closedir( dir );
	}
	*/
}
