#ifndef _CONVERT_H_
#define _CONVERT_H_

#include <time.h>
#include "common.h"

char* get_lcid_string(LCID i);
char* make_time_t_string(time_t i);
char* make_FILETIME_string(FILETIME i);

#endif /* _CONVERT_H_ */
