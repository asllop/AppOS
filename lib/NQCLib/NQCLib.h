#include "NQCLib_config.h"

#ifndef NQCLIB_H
#define	NQCLIB_H

#ifdef NQC_ITOA
char                        *itoa(int value, char *str, int base);
#endif
#ifdef NQC_ATOI
int                         atoi(const char *str);
#endif
#ifdef NQC_STRLEN
size_t                      strlen(const char *str);
#endif
#ifdef NQC_STRCPY
char                        *strcpy(char *dest, const char *src);
#endif
#ifdef NQC_MEMCPY
void                        *memcpy(void *destination, const void *source, size_t num);
#endif
#ifdef NQC_MEMSET
void                        *memset(void *ptr, int value, size_t num);
#endif

#endif
