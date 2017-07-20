#include "NQCLib_config.h"

#ifndef NQCLIB_H
#define	NQCLIB_H

#ifdef ITOA
char                        *itoa(int value, char *str, int base);
#endif
#ifdef ATOI
int                         atoi(const char *str);
#endif
#ifdef STRLEN
size_t                      strlen(const char *str);
#endif

#endif
