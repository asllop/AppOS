#ifndef APPOS_H
#define	APPOS_H

#ifndef NULL
#define NULL                (void *)0
#endif

#ifndef YES
#define YES                 (1==1)
#endif

#ifndef NO
#define NO                  (!YES)
#endif

#ifndef TRUE
#define TRUE                YES
#endif

#ifndef FALSE
#define FALSE				NO
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
    ERR_CODE_INPUTBUFFERMEM
    
} ERR_CODE;

typedef unsigned int        	TASK;
typedef unsigned int            PRIORITY;
typedef unsigned char           EVENT;
typedef unsigned long long      TIME;

// TODO: define this on an arch header
typedef unsigned char			uint8_t;
typedef signed char				int8_t;
typedef unsigned short int 		uint16_t;
typedef signed short int 		int16_t;
typedef unsigned int 			uint32_t;
typedef signed int 				int32_t;
typedef unsigned long long 		uint64_t;
typedef signed long long 		int64_t;
typedef unsigned long			size_t;
typedef long                    time_t;

#ifndef __cplusplus
typedef char					bool;
#endif

#endif

