#ifndef _WINDOWS_H_
#define _WINDOWS_H_

extern DWORD num_windows;

bool open_windows( void );
bool print_windows_entry(FILE* hhp);
void close_windows( void );

#endif /* _WINDOWS_H_ */
