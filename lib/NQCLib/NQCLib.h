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

#endif
