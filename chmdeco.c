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
chmdeco.c - this module implements decompilation of CHMs
It was written by Pabs.
*/



/* Local headers */

#include "common.h"
#include "system.h"
#include "sitemap.h"
#include "strings_file.h"
#include "hhp.h"
#include "hhc.h"
#include "hhk.h"
#include "misc.h"



/* Options/Preferences */

bool lcid_success = false; /* For later, when its dumping is added */
bool print_defaults = false;
bool print_blurb = true;
bool print_stats = true;
bool print_entity_refs = false;
bool recreate_html = false;
bool process_options = true;



/* Parameters */

char* cwd = NULL;
char* input = NULL;

char* Compiled_file = NULL;
char* Project_file = NULL;
char* Contents_file = NULL;
char* Index_file = NULL;



void cleanup_on_exit( void );
void print_usage( void );

int main( int argc, char* argv[] ){
	int argi;
	size_t size;

	if( argc < 2 ) print_usage();

	/* Get the current dir so we can process multiple input dirs */
#if defined(__MINGW32__) || defined(_MSC_VER)
	size = MAX_PATH;
#else
	size = (size_t)pathconf(".", _PC_PATH_MAX);
#endif
	cwd = (char *)malloc(size);
	if (cwd){
		if( !getcwd(cwd, size)){
			fprintf( stderr, "%s: %s: %s\n", PACKAGE, "current directory", strerror(errno) );
			FREE(cwd);
		}
	} else fprintf( stderr, "%s: %s: %s\n", PACKAGE, "current directory buffer", strerror(errno) );
	if(!cwd) return -1;

	atexit(cleanup_on_exit);

	for( argi = 1; argi < argc; argi++ ){

		/* In case any arguments are relative filenames */
		chdir(cwd);

		/* FIXME: Use getopt for options processing? */
		if( process_options && *argv[argi] == '-' ){
			switch( argv[argi][1] ){
				case 'p': print_defaults = true; break;
				case 'b': print_blurb = false; break;
				case 's': print_stats = false; break;
				case 'e': print_entity_refs = true; break;
				case 'f': recreate_html = true; break;
				case '-': process_options = false; break;
				case 'h':
					print_usage();
				default:
					fprintf( stderr, "%s: invalid option -- %s\n", PACKAGE, argv[argi] );
			}
		} else {

			struct stat st;
			bool faked_idxhdr = false;

			/* Get a better string for the input name */
			input = strdup( argv[argi] ); /* In case the platform has read-only args */
			if( !input ) input = argv[argi];
			size = strlen(input) - 1;
			if( input[size] == '/' ) input[size] = 0;

			/* Check what we are dealing with */
			if( !stat( argv[argi], &st ) ){
				if( !S_ISDIR( st.st_mode ) ){
					fprintf( stderr, "%s: %s: %s\n", PACKAGE, input, "its dumping is not yet supported" );
					continue;
				}
			} else {
				fprintf( stderr, "%s: %s: %s\n", PACKAGE, input, strerror(errno) );
				/* Try & decompile anyway */
			}

			/* Enter the input dir */
			if( chdir(input) != 0 ){
				fprintf( stderr, "%s: %s: %s\n", PACKAGE, input, strerror(errno) );
				continue;
			}

			/*
			A folder to create our files in. Done in a subdir to prevent
			overwrites of any stuff/authoring files that may have been
			added to the chm.
			*/
			if( mkdir( "#recreated", 0777 ) != 0 && errno != EEXIST ){
				fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "#recreated", strerror(errno) );
				continue;
			}

			if( open_system() ){

				/* Fake the #IDXHDR file if possible */
				if( !exists("#IDXHDR") ){
					BYTE* system_IDXHDR = get_system(IDXHDR_FILE_CODE);
					if(system_IDXHDR){
						FILE* idxhdr = fopen("#IDXHDR","wb");
						if(idxhdr){
							if( fwrite(system_IDXHDR,system_entries[IDXHDR_FILE_CODE].length,1,idxhdr) )
								faked_idxhdr = true;
							else
								fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "#IDXHDR", strerror(errno) );
							FCLOSE(idxhdr);

							/* Don't want a bogus file */
							if( !faked_idxhdr ) remove("#IDXHDR");
						} else
							fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "#IDXHDR", strerror(errno) );
					}
				}

				/* Make the hhp output name */
				Compiled_file = (char*)get_system(Compiled_file_CODE);
				/* This is more accurate since a dump may go anywhere */
				if( Compiled_file ){
					size_t Compiled_file_len = strlen(Compiled_file);
					size_t Project_file_len = Compiled_file_len + sizeof(".hhp");
					Project_file = (char*)malloc(Project_file_len);
					if( Project_file ){
						strcpy( Project_file, Compiled_file );
						strcpy( &Project_file[Compiled_file_len], ".hhp" );
					} else
						fprintf( stderr, "%s: %s %s: %s\n", PACKAGE, input, "project name buffer", strerror(errno) );
				}

				/* Make the hhc output name */
				Contents_file = (char*)get_system(Contents_file_CODE);
				if( !Contents_file ){
					DWORD Binary_TOC;
					if( get_system_DWORD(Binary_TOC_CODE,&Binary_TOC) ){
						if( open_sitemap() ){
							Binary_TOC = find_in_urltbl(Binary_TOC);
							if( Binary_TOC ) Contents_file = get_urlstr( Binary_TOC+8 );
							close_sitemap();
						}
					}
				}

				/* Make the hhk output name */
				Index_file = (char*)get_system(Index_file_CODE);
				if( !Index_file ){
					DWORD Binary_Index;
					if( get_system_DWORD(Binary_Index_CODE,&Binary_Index) ){
						if( open_sitemap() ){
							Binary_Index = find_in_urltbl(Binary_Index);
							if( Binary_Index ) Index_file = get_urlstr( Binary_Index+8 );
							close_sitemap();
						}
					}
				}

				/* Don't close the #SYSTEM just yet - needed below */
			}

			/* Second choice for the hhp name */
			if( !Project_file ){
				size_t Project_stem_len = size;

				if( 0 == strncasecmp( ".chm", &input[Project_stem_len-4] , 4 ) )
					Project_stem_len -= 4;

				Project_file = (char*)malloc(Project_stem_len+sizeof(".hhp"));

				if( Project_file ){
					strncpy( Project_file, input, Project_stem_len );
					strcpy( &Project_file[Project_stem_len], ".hhp" );
				} else
					/* The modules that need this have their own defaults */
					fprintf( stderr, "%s: %s %s: %s\n", PACKAGE, input, "project name buffer", strerror(errno) );
			}

			/* Recreate context sensitivity files */
			recreate_alias_map();

			/* The list of words that are not entered into the full-text search database */
			recreate_fts_stop_list_file();

			/* The project file (options, window types, files, merge ...) */
			recreate_hhp();
			close_system();
			if( Project_file ){ FREE(Project_file); }
			if( Compiled_file ){ FREE(Compiled_file); }

			/* The table of contents */
			recreate_hhc();
			if( Contents_file ){ FREE(Contents_file); }

			/* The index */
			recreate_hhk();
			if( Index_file ){ FREE(Index_file); }

			/* FIXME: Implement the following
#ifdef WITH_EDITING_HTML_PARSER
			/ * This adds any Alinks() (minus MS-HAID ones) found to
			html files and may perhaps detect & fix Split html files * /
			process_html_files();
#endif*/

			if( recreate_html )
				recreate_html_from_fts();

			close_strings();

			/* Clean up the mess we made */
			if( faked_idxhdr ) remove("#IDXHDR");

		}
	}

	cleanup_on_exit();

	return 0;
}

void cleanup_on_exit( void ){
	FREE(cwd);
}

void print_usage( void ){
	fprintf( stderr, "%s %s\n", PACKAGE, VERSION );
	fprintf( stderr, "Usage: %s (options|directory)...\n", PACKAGE );
	fprintf( stderr, "\t-p\n\t\tTurn on printing defaults in the [OPTIONS] section of the hhp\n" );
	fprintf( stderr, "\t-b\n\t\tTurn off printing the blurb at the start of the hhp\n" );
	fprintf( stderr, "\t-s\n\t\tTurn off printing the compilation stats at the start of the hhp\n" );
	fprintf( stderr, "\t-v\n\t\tPrint version and usage on stderr, then exit\n" );
	fprintf( stderr, "\t--\n\t\tTurn off options processing for the rest of the arguments\n" );
	fprintf( stderr, "\nCopyright (C) 2003 %s %s\nThis free software, distributed under the GNU GPL.\n", AUTHOR, AUTHOR_EMAIL );
	fprintf( stderr, "See the source for more details and note there is NO WARRANTY of ANY kind.\n" );
	fprintf( stderr, "See the website for updates: %s\n", WEBSITE );
	exit(0);
}
