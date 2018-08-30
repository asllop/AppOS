#ifndef APPOS_H
#define	APPOS_H

// GCC defined types
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef YES
#define YES                 true
#endif

#ifndef NO
#define NO                  false
#endif

#ifndef TRUE
#define TRUE                true
#endif

#ifndef FALSE
#define FALSE               false
#endif

#ifndef __unused
#define __unused            __attribute__((unused))
#endif

// Versioning rules: http://semver.org/

#define VERSION_MAJOR       0
#define VERSION_MINOR       1
#define VERSION_PATCH       0

typedef enum
{
    ERR_CODE_TASKCNTOVR     = -512,
    ERR_CODE_TASKCNTUND,
    ERR_CODE_NOTASKID,
    ERR_CODE_BADTASKSTATE,
    ERR_CODE_EVENTFULL,
    ERR_CODE_NOEVENT,
    ERR_CODE_MUTEXWHILEFORBID,
    ERR_CODE_BADSERIALCONFIG,
    ERR_CODE_NOTERMDRIVER,
    ERR_CODE_TERMDRIVERCNTOVR,
    ERR_CODE_BADIPVERSION,
    ERR_CODE_BADIPHEADERSIZE,
    ERR_CODE_BADIPPACKETSIZE,
    ERR_CODE_BADIPCHECKSUM,
    ERR_CODE_NOMEMFORIPPACKET,
    ERR_CODE_IPBUFFEREMPTY,
    ERR_CODE_NOPACKETSLOTS,
    ERR_CODE_IPV4BADCHECK,
    ERR_CODE_BADSOCKTYPE,
    ERR_CODE_NOSOCKETFREE,
    ERR_CODE_NOSOCKETFOUND,
    
} ERR_CODE;

typedef unsigned int            TASK;
typedef unsigned int            PRIORITY;
typedef unsigned long long      TIME;
typedef long                    time_t;
typedef unsigned char           byte;

#endif

