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
misc.c - this module implements stp, context sensitivity, samples, html from fts
output.
It was written by Pabs.
*/



/* Local headers */

#include "chmdeco.h"
#include "common.h"
#include "sitemap.h"
#include "strings_file.h"
#include "misc.h"



bool stp_success = false;

/* FIXME: output the stp 1 word per line, all on one line or a limited number of chars/words per line? */
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
															fputs( STP_SEPARATOR, stp );
														}

														if( ferror(stp) )
															fprintf( stderr, "%s: %s/%s/%s: %s\n", PACKAGE, input, "#recreated", FTS_STOP_LIST_FILE_NAME, strerror(errno) );
														else stp_success = true;

														FCLOSE(stp);

														/* We're done */
														break;

													} else fprintf( stderr, "%s: %s/%s/%s: %s\n", PACKAGE, input, "#recreated", FTS_STOP_LIST_FILE_NAME, strerror(errno) );
												} else fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "$OBJINST", strerror(errno) );
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
	fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "$OBJINST", strerror(errno) );
	FCLOSE(objinst);
	return;

MERROR:
	fprintf( stderr, "%s: %s %s: %s\n", PACKAGE, input, "stp buffer", strerror(errno) );
	FCLOSE(objinst);
}

bool alias_map_success = false;

void recreate_alias_map( void ){
	FILE* ivb;

	errno = 0;
	alias_map_success = false;

	if( !open_strings() ) return;

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
						fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "#IVB", strerror(errno) );
						break;
					}

					if( feof(ivb) ) break;
				}

				alias_map_success = true;

				if( ferror(map) ){
					alias_map_success =false;
					fprintf( stderr, "%s: %s/%s/%s: %s\n", PACKAGE, input, "#recreated", MAP_FILE_NAME, strerror(errno) );
				}

				if( ferror(alias) ){
					alias_map_success =false;
					fprintf( stderr, "%s: %s/%s/%s: %s\n", PACKAGE, input, "#recreated", ALIAS_FILE_NAME, strerror(errno) );
				}

				FCLOSE(map);
			} else fprintf( stderr, "%s: %s/%s/%s: %s\n", PACKAGE, input, "#recreated", MAP_FILE_NAME, strerror(errno) );
			FCLOSE(alias);
		} else fprintf( stderr, "%s: %s/%s/%s: %s\n", PACKAGE, input, "#recreated", ALIAS_FILE_NAME, strerror(errno) );
		FCLOSE(ivb);
	} else if( errno && errno != ENOENT )
		fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "#IVB", strerror(errno) );

}

bool samples_success = false;
char Sample_Staging_Path[8] = "samples";

void recreate_samples( void ){
	samples_success = false;
	fprintf( stderr, "%s: warning: %s: %s\n", PACKAGE, input, "samples decompilation not yet implemented" );
	/*
	FIXME: Implement this bit
	DIR* dir = opendir( "." );
	if( dir ){
		struct stat st;
		struct dirent* de;
		while( (de = readdir( dir )) ){
			if( !strcasecmp( de->d_name, "samples" ){
				if( !stat( de->d_name, &st ) && S_ISDIR( st.st_mode ) ){
					if a folder named samples has any *.sfl in it and a html file has a sample obj in it
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

/*
This reads the full text search information, gathering words and locations.
It then dumps all the words back into html files
*/

typedef struct _document_t {
	char** words;
	size_t words_len;
	size_t words_size;
	SRINT latest_title;
	SRINT earliest_non_title;
} document_t;

/*
replacement for mkdir -p
This needs read-write memory,
paths containing dir seps / and or \,
on failure it is evil
*/
int mkdirp(char* path){
	char savec;
	char* start = path;
	char* end = start;
	int numdirs = 0, i;
	int r = 0;
	for(;*end;){
		end = strpbrk( start, "/\\" ); /* Find the next dir end */
		if( end == start ){ start = ++end; continue; } /* On to the next one if it is a construct like // */
		if( end == NULL ) break; /* Ignore the filename */
		savec = *end; *end = '\0'; /* mkdir & chdir need terminater after the name */
		r = chdir( start ); /* Check if the dir exists */
		if( r == -1 ){
			r = mkdir( start, 0777 ); /* Create it if it doesn't */
			if( r == -1 ) { *end = savec; return -1; } /* Exit if we can't */
			chdir( start ); /* Enter it if we can */
		}
		*end = savec; /* Restore the dir separator */
		start = ++end; /* And on to the next dir */
		numdirs++; /* We did another dir */
	}
	for( i = 0; i < numdirs; i++ ) chdir( ".." );
	return 0;
}

void recreate_html_from_fts( void ){
	long fts_size;
	FILE* fiftimain = fopen( "$FIftiMain", "rb" );
	if( fiftimain ){
		if( !fseek( fiftimain, 0, SEEK_END ) && 0 < (fts_size = ftell( fiftimain )) && !fseek( fiftimain, 0, SEEK_SET ) ){

			chdir( "#recreated" );
			if( !mkdir( "#from-fts", 0777 ) || errno == EEXIST ){

#define FTS_HEADER_LEN 0x32
				BYTE header[FTS_HEADER_LEN] = { 0 };
				DWORD node_offset = 0;
				WORD tree_depth;
				/* _s = scale, _r = root */
				BYTE doc_index_s, doc_index_r;
				BYTE code_count_s, code_count_r;
				BYTE loc_codes_s, loc_codes_r;
				BYTE* leaf_node = NULL;
				BYTE* offset = NULL;
				BYTE old_len, new_len;
				DWORD node_len;
				DWORD tmp;
				WORD free_space;

				size_t current_word_grow = 50;
				char* current_word = NULL;
				size_t current_word_len = 0;
				size_t current_word_size = 0;

				size_t words_grow = 25000;
				char** words = NULL;
				size_t words_len = 0;
				size_t words_size = 0;
				size_t words_i = 0;

				SRINT document_index;
				SRINT code_count;
				SRINT location_code;

				size_t word_location_codes_grow = 5000;
				BYTE* word_location_codes = NULL;
				size_t word_location_codes_len = 0;
				size_t word_location_codes_size = 0;

				DWORD num_topics;
				uint word_i;

				BYTE context;
				ENCINT wlc_count;
				DWORD wlc_offset;
				ENCINT wlc_length;

				int wlc_bit;
				BYTE* wlc_p;

				uint di_i;
				uint lc_i;

				FILE* f;

				size_t docs_grow = 5000;
				document_t* docs = NULL;
				size_t docs_len = 0;
				size_t docs_size = 0;
				size_t docs_i = 0;

				size_t doc_words_grow = 500;

				chdir( ".." );

				/* Read the header */
				if( !fread( header, FTS_HEADER_LEN, 1, fiftimain ) ){
					fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "$FIftiMain", feof( fiftimain ) ? "unexpected end of file" : strerror(errno) );
					goto E_MALLOC;
				}

				tmp = get_DWORD( header + 8 );
				node_offset = get_DWORD( header + 0x14 );

				if( tmp != node_offset ){
					fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "$FIftiMain", "the two root index offsets are not equal, dunno which to trust" );
					EMAIL_CHM;
					goto E_FCLOSE;
				}

				doc_index_s = header[0x1E];
				doc_index_r = header[0x1F];
				code_count_s = header[0x20];
				code_count_r = header[0x21];
				loc_codes_s = header[0x22];
				loc_codes_r = header[0x23];

				if( doc_index_s != 2 || code_count_s != 2 || loc_codes_s != 2 ){
					fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "$FIftiMain", "one of the scale parameters has a value other than two, which is not yet supported" );
					EMAIL_CHM;
					goto E_FCLOSE;
				}

				node_len = get_DWORD( header + 0x2E );
				leaf_node = malloc(node_len);

				if( !leaf_node ){
					fprintf( stderr, "%s: %s %s: %s\n", PACKAGE, input, "leaf node buffer", strerror(errno) );
					goto E_FCLOSE;
				}

				num_topics = get_DWORD( header + 4 );
				tree_depth = get_WORD( header + 0x18 );

				/* Descend the index tree */
				fseek( fiftimain, node_offset, SEEK_SET );
				while( --tree_depth ){
					int len;
					fseek( fiftimain, 2, SEEK_CUR );
					len = fgetc( fiftimain );
					if( len == EOF ){
						fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "$FIftiMain", feof( fiftimain ) ? "unexpected end of file" : strerror(errno) );
						goto E_MALLOC;
					}
					fseek( fiftimain, (unsigned)len, SEEK_CUR );
					if( !read_DWORD( fiftimain, &node_offset ) ){
						fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "$FIftiMain", feof( fiftimain ) ? "unexpected end of file" : strerror(errno) );
						goto E_MALLOC;
					}
					fseek( fiftimain, node_offset, SEEK_SET );
				}

				/* Read all the words into memory */
				for(;;){

					if( !fread( leaf_node, node_len, 1, fiftimain ) ){
						fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "$FIftiMain", feof( fiftimain ) ? "unexpected end of file" : strerror(errno) );
						goto E_MALLOC;
					}

					node_offset = get_DWORD( leaf_node );
					tmp = get_WORD( leaf_node + 4 );
					free_space = get_WORD( leaf_node + 6 );

					if( tmp ){
						fprintf( stderr, "%s: warning: %s/%s: %s\n", PACKAGE, input, "$FIftiMain", "an unknown field was non-zero" );
						EMAIL_CHM;
					}

					if( current_word ) memset( current_word, 0, current_word_len );

					offset = leaf_node + 8;

					for( word_i = 0; offset < leaf_node+node_len-free_space; word_i++ ){

						new_len = *(offset++);
						old_len = *(offset++);

						if( !word_i && old_len ){
							fprintf( stderr, "%s: warning: %s/%s: %s\n", PACKAGE, input, "$FIftiMain", "old_len was non-zero at the start of a node" );
							EMAIL_CHM;
						}

						{ /* Grow the current_word string */
							size_t new_current_word_len = old_len+new_len;
							size_t new_current_word_size = ((new_current_word_len/current_word_grow)+1)*current_word_grow;
							if( new_current_word_size > current_word_size ){
								char* new_current_word = (char*)realloc(current_word,new_current_word_size);
								if( !new_current_word ){
									fprintf( stderr, "%s: %s %s: %s\n", PACKAGE, input, "current word buffer", strerror(errno) );
									goto E_MALLOC;
								}
								current_word = new_current_word;
								current_word_size = new_current_word_size;
							}
							current_word_len = new_current_word_len;
						}

						if(new_len){
							memcpy( current_word+old_len, offset, new_len-1 );
							offset += new_len-1;
						} else {
							fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "$FIftiMain", "zero new_len" );
							goto E_MALLOC;
						}
						if(old_len+new_len) current_word[old_len+new_len-1] = 0;

						{ /* Grow the word array */
							size_t new_words_len = words_len + 1;
							size_t new_words_size = ((new_words_len/words_grow)+1)*words_grow;
							if( new_words_size > words_size ){
								char** new_words = (char**)realloc(words,sizeof(char*)*new_words_size);
								if( !new_words ){
									fprintf( stderr, "%s: %s %s: %s\n", PACKAGE, input, "word array buffer", strerror(errno) );
									goto E_MALLOC;
								}
								words = new_words;
								words_size = new_words_size;
							}
							words_len = new_words_len;
						}

						/* Store the current word */
						words[words_len-1] = strdup(current_word);
						if( !words[words_len-1] ){
							fprintf( stderr, "%s: %s %s: %s\n", PACKAGE, input, "word buffer", strerror(errno) );
							goto E_MALLOC;
						}

						context = *(offset++);
						if( context != 0 && context != 1 ){
							fprintf( stderr, "%s: warning: %s/%s: %s\n", PACKAGE, input, "$FIftiMain", "unknown context" );
							EMAIL_CHM;
						}

						wlc_count = get_be_ENCINT( &offset );
						wlc_offset = get_DWORD( offset ); offset += 4;
						tmp = get_WORD( offset ); offset += 2;
						if( tmp ){
							fprintf( stderr, "%s: warning: %s/%s: %s\n", PACKAGE, input, "$FIftiMain", "an unknown field was non-zero" );
							EMAIL_CHM;
						}
						wlc_length = get_be_ENCINT( &offset );

						{ /* Grow the word location codes buffer */
							size_t new_word_location_codes_len = wlc_length;
							size_t new_word_location_codes_size = ((new_word_location_codes_len/word_location_codes_grow)+1)*word_location_codes_grow;
							if( new_word_location_codes_size > word_location_codes_size ){
								BYTE* new_word_location_codes = (BYTE*)realloc(word_location_codes,new_word_location_codes_size);
								if( !new_word_location_codes ){
									fprintf( stderr, "%s: %s %s: %s\n", PACKAGE, input, "encoded word locations buffer", strerror(errno) );
									goto E_MALLOC;
								}
								word_location_codes = new_word_location_codes;
								word_location_codes_size = new_word_location_codes_size;
							}
							word_location_codes_len = new_word_location_codes_len;
						}

						fseek( fiftimain, wlc_offset,  SEEK_SET );
						if( !fread( word_location_codes, wlc_length, 1, fiftimain ) ){
							fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "$FIftiMain", feof( fiftimain ) ? "unexpected end of file" : strerror(errno) );
							goto E_MALLOC;
						}

						wlc_bit = 7;
						wlc_p = word_location_codes;
						document_index = 0;

						for( di_i = 0; di_i < wlc_count && wlc_p < word_location_codes+wlc_length; di_i++ ){

							/* Entries are padded to a full byte (usually with zeros) */
							if( wlc_bit != 7 ) wlc_p++;
							wlc_bit = 7;

							document_index += get_SRINT( &wlc_p, &wlc_bit, doc_index_s, doc_index_r );
							code_count = get_SRINT( &wlc_p, &wlc_bit, code_count_s, code_count_r );

							{ /* grow document array if nessecary */
								size_t new_docs_len = document_index + 1;
								size_t new_docs_size = ((new_docs_len/docs_grow)+1)*docs_grow;
								if( new_docs_size > docs_size ){
									document_t* new_docs = (document_t*)realloc(docs,sizeof(document_t)*new_docs_size);
									if( !new_docs ){
										fprintf( stderr, "%s: %s %s: %s\n", PACKAGE, input, "document array buffer", strerror(errno) );
										goto E_MALLOC;
									}
									docs = new_docs;
									docs_size = new_docs_size;
								}
								/* Initialize the new entries */
								if( new_docs_len > docs_len ){
									for( docs_i = docs_len; docs_i < new_docs_len; docs_i++ ){
										docs[docs_i].words = NULL;
										docs[docs_i].words_len = docs[docs_i].words_size = 0;
										docs[docs_i].latest_title = docs[docs_i].earliest_non_title = ~(SRINT)0;
									}
									docs_len = new_docs_len;
								}
							}

							location_code = 0;

							for( lc_i = 0; lc_i < code_count; lc_i++ ){

								location_code += get_SRINT( &wlc_p, &wlc_bit, loc_codes_s, loc_codes_r );

								{ /* grow word array if nessecary */
									size_t new_words_len = location_code + 1;
									size_t new_words_size = ((new_words_len/doc_words_grow)+1)*doc_words_grow;
									if( new_words_size > docs[document_index].words_size ){
										char** new_words = (char**)realloc(docs[document_index].words,sizeof(char*)*new_words_size);
										if( !new_words ){
											fprintf( stderr, "%s: %s %s: %s\n", PACKAGE, input, "document word array buffer", strerror(errno) );
											goto E_MALLOC;
										}
										docs[document_index].words = new_words;
										docs[document_index].words_size = new_words_size;
									}
									if( new_words_len > docs[document_index].words_len ){
										/* memset( &docs[document_index].words[docs[document_index].words_len-1], 0, sizeof(docs[document_index].words[0])*(new_words_len-docs[document_index].words_len-1) ); */
										for( words_i = docs[document_index].words_len; words_i < new_words_len; words_i++ )
											docs[document_index].words[words_i] = NULL;
										docs[document_index].words_len = new_words_len;
									}
								}

								/* Initialize this entry */
								docs[document_index].words[location_code] = words[words_len-1];

								if( context ){
									if( docs[document_index].latest_title == ~(SRINT)0 || docs[document_index].latest_title < location_code )
										docs[document_index].latest_title = location_code;
								} else if( docs[document_index].earliest_non_title == ~(SRINT)0 || docs[document_index].earliest_non_title > location_code )
									docs[document_index].earliest_non_title = location_code;
							}
						}
					}

					if( !node_offset ) break;

					fseek( fiftimain, node_offset, SEEK_SET );
				}


				if( open_sitemap() ){
					chdir( "#recreated" );
					chdir( "#from-fts" );
					/* Write out all the words into documents */
					for( docs_i = 0; docs_i < docs_len; docs_i++ ){

						/* Long enough for either a #TOPICS entry (16) or a sprintf( "#%d.html", int 64 ) */
	/* #if ( sizeof(SRINT) > 4)
		#error You need to increase the size of topic on the next line
	#end */
						BYTE entry[26];

						char* out_file = NULL;

						if( !docs[docs_i].words ) continue;

						if( docs[docs_i].earliest_non_title != ~(SRINT)0 && docs[docs_i].latest_title != ~(SRINT)0 && docs[docs_i].earliest_non_title <= docs[docs_i].latest_title ){
							fprintf( stderr, "%s: warning: %s/%s: %s\n", PACKAGE, input, "$FIftiMain", "a document had overlapping title & body" );
							EMAIL_CHM;
						}

						fseek( topics, docs_i*16, SEEK_SET );
						if( fread( entry, 16, 1, topics ) ){
							fseek( urltbl, get_DWORD(entry+8)+8, SEEK_SET);
							if( read_DWORD(urltbl,&tmp) ){
								out_file = get_urlstr(tmp+8);
							}
						}

						/* Default in case of errors*/
						if( !out_file || !*out_file ){
							if( out_file ) free( out_file );
							sprintf( entry, "#%d.html", docs_i );
							out_file = entry;
						}

						mkdirp( out_file );
						f = fopen( out_file, "wb" );
						if( f ){

							fputs( "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\"><html>\n", f );

								fputs( "<head>\n", f );
									fputs( "<title>", f );
										print_string( f, get_DWORD(entry+4) );
									fputs( "</title>", f );
									if( docs[docs_i].latest_title != ~(SRINT)0 ){
										fputs( "\n<title>", f );
											for( word_i = 0 ; word_i < docs[docs_i].latest_title; word_i++ ){
												fputs( docs[docs_i].words[word_i] ? docs[docs_i].words[word_i] : "????", f );
												fputc( ' ', f );
											}
											fputs( docs[docs_i].words[word_i] ? docs[docs_i].words[word_i] : "????", f );
										fputs( "</title>", f );
									}
								fputs( "\n</head>\n", f );

								if( docs[docs_i].earliest_non_title != ~(SRINT)0 ){
									fputs( "<body><p>\n", f );
										for( word_i = docs[docs_i].earliest_non_title; word_i < docs[docs_i].words_len; word_i++ ){
											fputs( docs[docs_i].words[word_i] ? docs[docs_i].words[word_i] : "????", f );
											fputc( '\n', f );
										}
									fputs( "</p></body>\n", f );
								}

							fputs( "</html>", f );

							if( ferror( f ) | feof( f ) ){
								fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "$FIftiMain", feof( fiftimain ) ? "unexpected end of file" : strerror(errno) );
							}

							fclose( f );
							f = NULL;
						} else {
							fprintf( stderr, "%s: warning: %s: %s\n", PACKAGE, out_file, strerror(errno) );
						}

						FREE(docs[docs_i].words);

						if( out_file != (char*)entry ){ FREE( out_file); }
					}
					chdir( ".." );
					chdir( ".." );
					close_sitemap();
				}


E_MALLOC:
				for(docs_i = 0; docs_i < docs_len; docs_i++ )
					free(docs[docs_i].words);
				free(docs);
				for(words_i = 0; words_i < words_len; words_i++ )
					free(words[words_i]);
				free(words);
				free(word_location_codes);
				free(current_word);
				free(leaf_node);
			} else chdir( ".." );
		}
E_FCLOSE:
		fclose( fiftimain );
	} else if( errno && errno != ENOENT )
		fprintf( stderr, "%s: %s/%s: %s\n", PACKAGE, input, "$FIftiMain", strerror(errno) );

}
