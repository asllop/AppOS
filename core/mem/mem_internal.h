#include <appos.h>

#ifndef MEM_INTERNAL_H
#define	MEM_INTERNAL_H

#ifndef MAX_NUM_BLOCKS
#define MAX_NUM_BLOCKS      1
#endif

struct BlockStruct
{
    void                    *block;
    size_t                  totalSize;
    size_t                  usedSize;
    struct AllocStruct      *first;
    struct AllocStruct      *last;
};

struct AllocStruct
{
    void                    *buffer;
    size_t                  totalSize;
    unsigned char           hole;
    struct BlockStruct      *block;
    struct AllocStruct      *next;
    struct AllocStruct      *prev;
};

void                        mem_init();
struct BlockStruct          *get_blocks(int *num);
struct AllocStruct          *find_first_alloc(struct BlockStruct *block);
struct AllocStruct          *find_last_alloc(struct BlockStruct *block);
struct AllocStruct          *find_next_alloc(struct AllocStruct *allocBuffer);
struct AllocStruct          *find_prev_alloc(struct AllocStruct *allocBuffer);
int                         fast_free(void *buf);

/* Architecture dependant functions */

void                        setup_mem();
int                         scan_blocks(struct BlockStruct *blockArray);

#endif
