#include <appos.h>

#ifndef MEM_INTERNAL_H
#define	MEM_INTERNAL_H

#ifndef MAX_NUM_BLOCKS
#define MAX_NUM_BLOCKS      1
#endif

#ifndef SEGMENT_SIZE
#define SEGMENT_SIZE        256
#endif

struct BlockStruct
{
    void                    *block;
    size_t                  blockSize;
};

typedef uint16_t            SEGMENT;

void                        mem_init();
struct BlockStruct          *get_blocks(byte *num);
void                        internal_free(void *buf);

/* Architecture dependant functions */

void                        setup_mem();
int                         scan_blocks(struct BlockStruct *blockArray);

#endif
