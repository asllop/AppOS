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

typedef enum
{
    ERR_CODE_MEMCOPY        = -512,
    ERR_CODE_RELEASE,
    ERR_CODE_NOTASKSLOT,
    ERR_CODE_TASKCNTOVR,
    ERR_CODE_TASKCNTUND,
    ERR_CODE_NOTASKID,
    ERR_CODE_EVENTFULL,
    ERR_CODE_NOEVENT,
    ERR_CODE_BADCONSOLERES,
    ERR_CODE_FRAMEBUFFERMEM,
    ERR_CODE_INPUTBUFFERMEM,
    ERR_CODE_MUTEXWHILEFORBID,
    ERR_CODE_BADSERIALCONFIG,
    
} ERR_CODE;

typedef unsigned int            TASK;
typedef unsigned int            PRIORITY;
typedef unsigned char           EVENT;
typedef unsigned long long      TIME;
typedef long                    time_t;

#endif

