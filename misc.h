#ifndef _MISC_H_
#define _MISC_H_

#include "common.h"

#define FTS_STOP_LIST_FILE_NAME "fts-stop-list-file.stp"
extern bool stp_success;
void recreate_fts_stop_list_file( void );

#define ALIAS_FILE_NAME "alias.ali"
#define MAP_FILE_NAME "map.h"
extern bool alias_map_success;
void recreate_alias_map( void );

#define Sample_list_file "Sample-list-file.hhs"
extern bool samples_success;
extern char Sample_Staging_Path[8];
void recreate_samples( void );

#endif /* _MISC_H_ */
