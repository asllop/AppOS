#include <lib/NQCLib/NQCLib_config.h>
#include <stdarg.h>

#ifndef NQCLIB_H
#define	NQCLIB_H

// TODO: implement the most important Std C Lib functions, that are the ones in stdio.h, stdlib.h, string.h and math.h

#ifdef NQC_ITOA
char                        *itoa(int value, char *str, int base);
#endif
#ifdef NQC_LTOA
char                        *ltoa(long N, char *str, int base);
#endif
#ifdef NQC_ULTOA
char                        *ultoa(unsigned long value, char *str, int base);
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
#ifdef NQC_STRNCPY
char                        *strncpy(char *dest, const char *src, size_t n);
#endif
#ifdef NQC_MEMCPY
void                        *memcpy(void *destination, const void *source, size_t num);
#endif
#ifdef NQC_MEMSET
void                        *memset(void *ptr, int value, size_t num);
#endif
#ifdef NQC_MEMCMP
int                         memcmp(const void* s1, const void* s2,size_t n);
#endif
#ifdef NQC_STRNCMP
int                         strncmp(const char *str1, const char *str2, size_t n);
#endif
#ifdef NQC_STRCMP
int                         strcmp(const char *s1, const char *s2);
#endif
#ifdef NQC_VSPRINTF
int                         vsprintf(char *buf, const char *fmt, va_list args);
#endif
#ifdef NQC_SPRINTF
int                         sprintf(char *buf, const char *fmt, ...);
#endif

#endif
