#ifndef _STRINGS_FILE_H_
#define _STRINGS_FILE_H_

#include "common.h"

bool open_strings( void );
bool print_string( FILE* f, off_t off );
char* get_string( off_t off );
void print_strings( FILE* f );
void close_strings( void );

#endif /* _STRINGS_FILE_H_ */
