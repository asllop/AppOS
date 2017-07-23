#ifndef NQCLIB_CONFIG_H
#define	NQCLIB_CONFIG_H

#ifndef NQC_STR_LIMIT
#define NQC_STR_LIMIT       50000
#endif

// Comment out the functions you don't need

#define NQC_ITOA            // Dependencies: NQC_LTOA
#define NQC_LTOA
#define NQC_ATOI
#define NQC_STRLEN
#define NQC_STRCPY
#define NQC_STRNCPY
#define NQC_MEMCPY
#define NQC_MEMSET
#define NQC_STRNCMP
#define NQC_STRCMP

#endif
