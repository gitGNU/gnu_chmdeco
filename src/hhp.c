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



/*
hhp.c - this module implements conversion of internal files into a .hhp project.
It was written by Paul Wise.
*/



/* Local headers */

#include "chmdeco.h"
#include "common.h"
#include "convert.h"
#include "windows.h"
#include "system.h"
#include "strings_file.h"
#include "sitemap.h"
#include "misc.h"
#include "hhp.h"



DWORD stamp;
LCID os_lcid;
LCID compiler_lcid;



char* prefix = NULL;
size_t prefix_size = 0;
size_t prefix_len = 0;
#define PREFIX_GROW 50
void print_files( char* d );



FILE* hhp = NULL;
bool hhp_success = false;



bool no_files_yet = true;



void recreate_hhp( void ){
	char* pf = Project_file ? Project_file : HHP_FILE_NAME;

	if( !open_strings() ) return;
	if( !open_system() ) return;

	hhp = recreate( pf );
	if( hhp ){

		/* Various stuff */
		bool got_stuff = false;
		LCID Language = 0;
		DWORD DBCS = 0;
		DWORD Full_text_search = 0;
		DWORD KLinks = 0;
		DWORD ALinks = 0;
		QWORD compilation_FILETIME = 0;
		{
			BYTE* stuff = get_system(Language_DBCS_FTS_KLinks_ALinks_FILETIME_CODE);
			if( stuff ){
				got_stuff = true;
				Language = get_DWORD(stuff);
				DBCS = get_DWORD(stuff+4);
				Full_text_search = get_DWORD(stuff+8);
				KLinks = get_DWORD(stuff+12);
				ALinks = get_DWORD(stuff+16);
				compilation_FILETIME = get_QWORD(stuff+20);
				FREE(stuff);
			}
		}

		if( print_blurb )
			fputs(
				";This HHP file was recreated by "PACKAGE" "VERSION" (by "AUTHOR" - "WEBSITE")\r\n"
				";It is only an approximation of the original project file.\r\n"
				";Other files that may have been recreated with it are also only approximations.\r\n"
				";See the documentation for parts of the HHP that cannot be recreated.\r\n\r\n", hhp
			);

		if( print_stats ){

			/* FIXME: Print something else when we get its dumping */
			if( input ){
				char* fn = strrchr( input, '/' );
				fprintf( hhp, ";Input: %s\r\n", fn ? fn+1 : input );
			}

			{ /* Compiler name */
				FILE* f;
				char* compiled_by = (char*)get_system(COMPILED_BY_CODE);
				if(compiled_by) fprintf( hhp, ";Compiled by: %s\r\n", compiled_by );
				FREE(compiled_by);
				f = fopen("#BSSC","rb");
				if( f != NULL ){
					char robohelp_version[100] = "";
					size_t len = 0;
					fprintf( hhp, ";Prepared using: RoboHelp version " );
					while( (len = fread( robohelp_version, 1, sizeof(robohelp_version)-1, f )) ){
						robohelp_version[len-1] = 0;
						fprintf( hhp, "%s", robohelp_version );
					}
					fprintf( hhp, "\r\n" );
					fclose(f);
				}
			}

			/* Compiler language */
			if( lcid_success ){
				char* compiler_lcid_string = get_lcid_string( compiler_lcid );
				if(compiler_lcid_string) fprintf( hhp, ";Compiler language: %s (0x%x)\r\n", compiler_lcid_string, compiler_lcid );
				else fprintf( hhp, ";Compiler language id: 0x%x \r\n", compiler_lcid );
			}

			{ /* Compilation date (time_t) */
				DWORD compilation_time_t = 0;
				if( get_system_DWORD(time_t_STAMP_CODE,&compilation_time_t) ){
					char* compilation_time_t_string = make_time_t_string(compilation_time_t);
					if(compilation_time_t_string) fprintf( hhp, ";Compilation date: %s (%u seconds after 0:00 Jan 1 1970)\r\n", compilation_time_t_string, compilation_time_t );
					else fprintf( hhp, ";Compilation date: %u seconds after 0:00 Jan 1 1970\r\n", compilation_time_t );
				}
			}

#if 0 /* This QWORD is totally fucked up */
			/* Compilation date (FILETIME) */
			if(got_stuff){
				char* compilation_FILETIME_string = make_FILETIME_string(compilation_FILETIME);
				if(compilation_FILETIME_string) fprintf( hhp, ";Compilation date: %s (%llu00 nano-seconds after 0:00 Jan 1 1600)\r\n", compilation_FILETIME_string, compilation_FILETIME );
				else fprintf( hhp, ";Compilation date: %llu00 nano-seconds after 0:00 Jan 1 1600\r\n", compilation_FILETIME );
				/* FREE(compilation_FILETIME_string); */
			}
#endif /* 0 */

			/* Operating system language */
			if( lcid_success ){
				char* os_lcid_string = get_lcid_string( os_lcid );
				if( os_lcid_string ) fprintf( hhp, ";Compilation operating system language: %s (0x%x)\r\n", os_lcid_string, os_lcid );
				else fprintf( hhp, ";Compilation operating system language id: 0x%x\r\n", os_lcid );
			}
		}

		fputs( "\r\n[OPTIONS]\r\n", hhp );

		/* Auto Index */
		if( got_stuff && (KLinks || print_defaults) )
			fprintf( hhp, "Auto Index=%s\r\n", KLinks ? "Yes" : "No" );

		/* If an option is commented out it means we can't get it from the internal files yet */

		/* Auto TOC */
		/* Warning this may be inaccurate if the hhp doesn't have auto toc, but auto-generated is in the hhc, for example by turning it off after a previous compilation */
		/* This is CHEATING
		{
			FIXME: find a minimalist html parser & flesh this one out :
			open old hhp if it is present & check for Auto TOC
			and or open html files & check for h1-h9 matching toc entries (binary or sitemap)
			old_Contents = find("*.hhc")
			FILE* old_hhc = fopen(old_Contents,"rb");
			if(old_hhc){
				parse the hhc looking for Auto Generated in the text/site properties object
				if( Auto_Generated ){
					Auto_TOC = 0;
					parse the hhc looking for the maximum next level of ul/li elements
					fprintf( hhp, ";WARNING: The way we get Auto TOC is fairly dodgy\r\nAuto TOC=%u\r\n", Auto_TOC );
				}
			}
		}*/

		{ /* Binary Index */
			/* Buggy when no HHK */
			bool Binary_Index = (system_entries[Binary_Index_CODE].offset >=0);
			if( !Binary_Index || print_defaults )
				fprintf( hhp, "Binary Index=%s\r\n", Binary_Index ? "Yes" : "No" );
		}

		{ /* Binary TOC */
			/* Buggy when no HHC */
			bool Binary_TOC = (system_entries[Binary_TOC_CODE].offset>=0);
			if( Binary_TOC || print_defaults )
				fprintf( hhp, "Binary TOC=%s\r\n", Binary_TOC ? "Yes" : "No" );
		}

		/* fprintf( hhp, "CITATION=%s\r\n", CITATION ); */
		/* fprintf( hhp, "COMPRESS=%u\r\n", COMPRESS ); */
		/* fprintf( hhp, "COPYRIGHT=%s\r\n", COPYRIGHT ); */

		{ /* Compatibility */
			/*
			FIXME: take the following into account?
				absence of system codes 7, 11, 12, 13, 15 and #TOCIDX, $WWKeywordlinks, $WWAssociativeLinks
				size of the #WINDOWS entries
			*/
			bool Compatibility = system_version > 2;
			if( !Compatibility || print_defaults )
				fprintf( hhp, "Compatibility=%s\r\n", Compatibility ? "1.1 or later" : "1.0" );
		}

		/* Compiled file */
		if(Compiled_file){
			fprintf( hhp, "Compiled file=%s.chm\r\n", Compiled_file );
			FREE(Compiled_file);
		}

		/* Contents file */
		if( Contents_file ) fprintf( hhp, "Contents file=%s\r\n", Contents_file );
		/* else {
			FIXME: Find out how to find files with a particular extension
			fputs( ";The best guess for the contents file is one of the following\r\n", hhp );
			while(f=findfile("*.hhc",case insensitive))
				fprintf( hhp, ";Contents file=%s\r\n", f );
		} */

		{ /* Create CHI file */
			bool Create_CHI_file = exists("#SYSTEM") && exists("$OBJINST") && exists("$FIftiMain") &&
				!exists("$WWAssociativeLinks") && !exists("$WWKeywordLinks") &&
				!exists("#IDXHDR") && !exists("#TOCIDX") && !exists("#URLSTR") &&
				!exists("#WINDOWS") && !exists("#ITBITS") && !exists("#STRINGS") &&
				!exists("#TOPICS") && !exists("#URLTBL");
			if( Create_CHI_file || print_defaults )
				fprintf( hhp, "Create CHI file=%s\r\n", Create_CHI_file ? "Yes" : "No" );
		}

		/* DBCS */
		if( got_stuff && (DBCS || print_defaults) )
			fprintf( hhp, "DBCS=%s\r\n", DBCS ? "Yes" : "No" );

		{ /* Default Font */
			char* Default_Font = (char*)get_system(Default_Font_CODE);
			if(Default_Font) fprintf( hhp, "Default Font=%s\r\n", Default_Font );
			FREE(Default_Font);
		}

		{ /* Default Window */
			char* Default_Window = (char*)get_system(Default_Window_CODE);
			if(Default_Window) fprintf( hhp, "Default Window=%s\r\n", Default_Window );
			FREE(Default_Window);
		}

		{ /* Default topic */
			char* Default_topic = (char*)get_system(Default_topic_CODE);
			if(Default_topic) fprintf( hhp, "Default topic=%s\r\n", Default_topic );
			FREE(Default_topic);
		}

		/* fprintf( hhp, "Display compile notes=%s\r\n", Display_compile_notes ? "Yes" : "No" ); */
		/* fprintf( hhp, "Display compile progress=%s\r\n", Display_compile_progress ? "Yes" : "No" ); */
		/* fprintf( hhp, "Enhanced decompilation=%s\r\n", Enhanced_decompilation ? "Yes" : "No" ); */
		/* fprintf( hhp, "Error log file=%s\r\n", Error_log_file ); */

		/*
		FIXME:
			check if urlstr contains repeated filenames (indicative of path stripping)?
			and there are no folders other than $WW*Links
		if( !Flat || print_defaults )
			fprintf( hhp, "Flat=%s\r\n", Flat ? "Yes" : "No" );
		*/

		/* Full text search stop list file */
		if(stp_success)
			fprintf( hhp, "Full text search stop list file=%s\r\n", FTS_STOP_LIST_FILE_NAME );

		{ /* Full-text search */
			bool got_fts = got_stuff;
			if( !got_fts ){
				if( !exists("$FIftiMain") ){ Full_text_search = false; got_fts = true; }
				else {
					struct stat st;
					if( stat("$FIftiMain",&st) == 0 ){
						got_fts = true;
						Full_text_search = (st.st_size != 0);
					}
				}
			}
			if( got_fts && (Full_text_search || print_defaults) )
				fprintf( hhp, "Full-text search=%s\r\n", Full_text_search ? "Yes" : "No" );
		}

		/* fprintf( hhp, "IGNORE=%s\r\n", IGNORE ); */

		/* Index file */
		if( Index_file ) fprintf( hhp, "Index file=%s\r\n", Index_file );
		/* else {
			FIXME: Find out how to find files with a particular extension
			fputs( ";The best guess for the Index file is one of the following\r\n", hhp );
			while(f=findfile("*.hhk",case insensitive))
				fprintf( hhp, ";Index file=%s\r\n", f );
		}*/

		{ /* Language */
			bool got_lcid = got_stuff;
			if(!got_lcid){
				FILE* tmp = fopen( "$FIftiMain", "rb" );
				if( tmp ){
					fseek( tmp, 0x7E, SEEK_SET );
					got_lcid = read_DWORD( tmp, &Language );
					FCLOSE( tmp );
				}
				if( !got_lcid && (tmp = fopen( "$WWKeywordLinks/BTree", "rb" )) ){
					fseek( tmp, 0x34, SEEK_SET );
					got_lcid = read_DWORD( tmp, &Language );
					FCLOSE( tmp );
				}
				if( !got_lcid && (tmp = fopen( "$WWAssociativeLinks/BTree", "rb" )) ){
					fseek( tmp, 0x34, SEEK_SET );
					got_lcid = read_DWORD( tmp, &Language );
					FCLOSE( tmp );
				}
				/* FIXME: check the LCIDs in $OBJINST */
				/* if( !got_lcid && (tmp = fopen( "$OBJINST", "rb" )) ){
					FCLOSE( tmp );
				} */
			}
			if(got_lcid){
				char* Language_string = get_lcid_string( Language );
				if( Language_string ) fprintf( hhp, "Language=0x%x %s\r\n", Language, Language_string );
				else fprintf( hhp, "Language=0x%x\r\n", Language );
			}
		}

		/* fprintf( hhp, "PREFIX=%s\r\n", PREFIX ); */

		 /* Sample Staging Path, Sample list file */
		if( samples_success ){
			fprintf( hhp, "Sample Staging Path=%s\r\n", Sample_Staging_Path );
			fprintf( hhp, "Sample list file=%s\r\n", Sample_list_file );
		}

		{ /* Title */
			char* Title = (char*)get_system( Title_CODE );
			if( Title ) fprintf( hhp, "Title=%s\r\n", Title );
			FREE( Title );
		}

		/* fprintf( hhp, "TMPDIR=%s\r\n", TMPDIR ); */

		{ /* Custom tab */
			BYTE* Custom_tabs = get_system( Custom_tabs_CODE );
			if(Custom_tabs){
				char* s = (char*)Custom_tabs+4;
				DWORD n = get_DWORD( Custom_tabs );
				while(n){
					if(!*s) break;
					fprintf( hhp, "Custom tab=\"%s\", ", s ); /* This is not a typo */
					s += strlen(s) + 1;
					if(!*s) break;
					fprintf( hhp, "%s\r\n", s );
					s += strlen(s) + 1;
					n --;
				}
				FREE(Custom_tabs);
			}
		}

		close_system();

		/* [WINDOWS] */
		if( open_windows() ){
			fputs( "\r\n[WINDOWS]\r\n", hhp );
			while( print_windows_entry( hhp ) );
			close_windows();
			/* FIXME: How to do the following properly
			uint i;
			bool got_one = true;
			for(i = 0; i < num_windows; i++){
				got_one = print_windows_entry( hhp );
				if(!got_one) break;
			}
			if(got_one){
				/ * So this is only printed when num_windows is a lie * /
				fputs( ";The following windows were hidden\r\n", hhp );
				while( print_windows_entry( hhp ) );
			} */
		}

		/* [FILES] */
		/* This only dumps out names of all non-internal files */
		no_files_yet = true;
		print_files(".");
		FREE(prefix);
		prefix_len = prefix_size = 0;

		/* [ALIAS], [MAP] */
		if( alias_map_success ){
			fprintf( hhp,
				"\r\n[ALIAS]\r\n"
				"#include %s\r\n"
				"\r\n[MAP]\r\n"
				"#include %s\r\n",
				ALIAS_FILE_NAME,
				MAP_FILE_NAME
			);
		}

		/* [TEXT POPUPS] */
		/*
		FIXME: For this we need to grep -l '^[[:space:]]*\.topic[[:space:]]\+[0-9]\+' *.txt | fileno(hhp)
		grep or glob()+regcomp() would be ideal, but, dunno if these can be relied on these for example on Win32
		the best would be if there were a grep to C script that made optimisations for constant regexps
		fputs( "\r\n[TEXT POPUPS]\r\n", hhp );
		*/

		{ /* [MERGE FILES] */
			FILE* idxhdr = fopen("#IDXHDR","rb");
			if( idxhdr ){
				DWORD num_merge_files;
				fseek( idxhdr, 0x48, SEEK_SET );
				if( read_DWORD( idxhdr, &num_merge_files ) && num_merge_files ){
					DWORD merge_file;
					fseek( idxhdr, 4, SEEK_CUR );
					fputs( "\r\n[MERGE FILES]\r\n", hhp );
					while( read_DWORD( idxhdr, &merge_file ) && merge_file ){
						print_string( hhp, merge_file );
						fputs( "\r\n", hhp );
					}
					if( merge_file ){
						fputs( ";The following chms were in the internal string list, but not the merge files list\r\n", hhp );
						/* stream the rest of the strings file through a \0 to \r\n converter */
						print_strings( hhp );
					}
				}
				FCLOSE( idxhdr );
			}
		}

		/* Output this or not? May as well, since HHW does */
		fputs( "\r\n[INFOTYPES]\r\n", hhp );

		{ /* [SUBSETS] */
			struct stat st;
			if( stat("#SUBSETS",&st) == 0 ){
				uint i, num_subsets = (st.st_size-4)/12;
				fputs( "\r\n[SUBSETS]\r\n", hhp );
				/* Here we want to print some string out once for each #SUBSETS entry */
				 /* FIXME: Rethink the strings below */
				if( num_subsets == 1 ) fprintf( hhp, "There was one entry in this section, but only garbage was produced\r\n" );
				else if( num_subsets )
					for( i = 0; i < num_subsets; i++ )
						fprintf( hhp, "There were %u entries in this section, but only garbage was produced\r\n", num_subsets );
				if( (st.st_size-4)%12 )
					fprintf( stderr, "%s: warning: %s/%s: %s\n", PACKAGE, input, "#SUBSETS", "partial entry found" );
			}
		}

		if( !ferror(hhp) ) hhp_success = true;
		else fprintf( stderr, "%s: %s/%s/%s: %s\n", PACKAGE, input, "#recreated", pf, strerror(errno) );

		FCLOSE( hhp );

	} else fprintf( stderr, "%s: %s/%s/%s: %s\n", PACKAGE, input, "#recreated", pf, strerror(errno) );
}

/* Returns true if the argument is the name of an internal file */
bool isinternal( char* f ){
	return *f == '#' || *f == '$';
}

/* Recurse through a dir, printing out the names of files found */
void print_files( char* d ){
	DIR* dir = opendir( d );
	if( dir ){

		struct stat st;
		struct dirent* de;

		while( (de = readdir( dir )) ){

			chdir( d );

			/* Most systems define these as the current & parent dirs */
			if( !strcmp( de->d_name, "." ) || !strcmp( de->d_name, ".." ) )
				goto BACKOUT;

			/* Ignore internal files, except when not in root */
			if( (!prefix || !*prefix) && isinternal(de->d_name) )
				goto BACKOUT;

			/* Check if it is a subdir */
			if( !stat( de->d_name, &st ) && S_ISDIR( st.st_mode ) ){

				/* Add the directory name */
				size_t old_prefix_len = prefix_len;
				size_t new_prefix_len = prefix_len + strlen( de->d_name ) + 1;
				size_t new_prefix_size = ((new_prefix_len+1)/PREFIX_GROW+1)*PREFIX_GROW;
				if( new_prefix_size > prefix_size ){
					char* new_prefix = (char*)realloc( prefix, new_prefix_size );
					if( !new_prefix ){
						fprintf( stderr, "%s: %s %s: %s\n", PACKAGE, input, "prefix buffer", strerror(errno) );
						goto BACKOUT;
					}
					prefix = new_prefix;
					prefix_size = new_prefix_size;
				}
				strcpy( prefix+prefix_len, de->d_name );
				prefix[new_prefix_len-1] = '\\';
				prefix[new_prefix_len] = '\0';
				prefix_len = new_prefix_len;

				/* Print any files in the subdir */
				print_files( de->d_name );

				/* Strip away the last path */
				prefix_len = old_prefix_len;
				prefix[prefix_len] = '\0';

			} else {

				/* FIXME: what is the best way to speed this up? */
				if( no_files_yet ){
					fputs( "\r\n[FILES]\r\n", hhp );
					no_files_yet = false;
				}

				/* Print the filename */
				fprintf( hhp, "%s%s\r\n", prefix?prefix:"", de->d_name );

			}

			/* Back out */
			BACKOUT:
				if( !!strcmp( d, "." ) ) chdir( ".." );
		}

		closedir( dir );
	} else
		fprintf( stderr, "%s: %s/%s/%s: %s\n", PACKAGE, input, prefix?prefix:"", d, strerror(errno) );
}
