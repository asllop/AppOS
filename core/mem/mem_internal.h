#ifndef MEM_INTERNAL_H
#define	MEM_INTERNAL_H

#include <appos.h>

#ifndef MAX_NUM_BLOCKS
#define MAX_NUM_BLOCKS      1
#endif

// TODO IMPROVEMENT: Create multiple blocks of memory, each one with different SEGMENT_SIZE,
// and allocate memory on a block with a SEGMENT_SIZE close to buffer size.
// This way we are wasting less memory.

#ifndef SEGMENT_SIZE
#define SEGMENT_SIZE        1024
#endif

struct BlockStruct
{
    void                    *block;
    size_t                  blockSize;
};

typedef uint16_t            SEGMENT;

// TODO: rename internal functions to add "mem_" prefix

void                        mem_init();
struct BlockStruct          *get_blocks(byte *num);
void                        internal_free(void *buf);
void                        *mem_move_offset(void *buf, size_t size, long offset);

/* Architecture dependant functions */

void                        setup_mem();
int                         scan_blocks(struct BlockStruct *blockArray);

#endif
