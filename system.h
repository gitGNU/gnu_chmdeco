#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "common.h"

enum SYSTEM_CODE {
	Contents_file_CODE = 0,
	Index_file_CODE,
	Default_topic_CODE,
	Title_CODE,
	Language_DBCS_FTS_KLinks_ALinks_FILETIME_CODE,
	Default_Window_CODE,
	Compiled_file_CODE,
	Binary_Index_CODE,
	ABBREVIATIONS_N_EXPLANATIONS_CODE,
	COMPILED_BY_CODE,
	time_t_STAMP_CODE,
	Binary_TOC_CODE,
	NUM_INFORMATION_TYPES_CODE,
	IDXHDR_FILE_CODE,
	Custom_tabs_CODE,
	INFORMATION_TYPE_CHECKSUM_CODE,
	Default_Font_CODE,
	MAX_CODE
};

struct system_entry{
	int offset;
	WORD length;
};

extern struct system_entry system_entries[MAX_CODE];
extern DWORD system_version;

bool open_system( void );
BYTE* get_system(WORD);
bool get_system_DWORD(WORD code, DWORD* d);
bool get_system_to(WORD code, BYTE* buf, WORD length);
void close_system( void );

#endif /* _SYSTEM_H_ */
