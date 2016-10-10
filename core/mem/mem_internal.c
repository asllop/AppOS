#include <mem/mem_internal.h>
#include <mem/mem.h>
#include <sys/sys.h>
#include <sys/sys_internal.h>

static struct BlockStruct   blocks[MAX_NUM_BLOCKS];
static int                  numBlocks;

void mem_init()
{
    setup_mem();
    
    if ((numBlocks = scan_blocks(blocks)) > 0)
    {
        // Create a hole for every block
        for (int i = 0 ; i < numBlocks ; i++)
        {
            struct AllocStruct *allocBuffer = (struct AllocStruct *)blocks[i].block;
            
            allocBuffer->block = &blocks[i];
            allocBuffer->buffer = blocks[i].block + sizeof(struct AllocStruct);
            allocBuffer->totalSize = blocks[i].totalSize - sizeof(struct AllocStruct);
            allocBuffer->hole = YES;
            allocBuffer->next = NULL;
            allocBuffer->prev = NULL;
            
            // Copy allocBuffer at the start of current block
            if (core_copy(allocBuffer, blocks[i].block, sizeof(struct AllocStruct)) < 0)
            {
                // Could not copy memory
                core_fatal("Could not init dynamic memory structure");
            }
            
            // Update block's node info
            blocks[i].first = blocks[i].last = (struct AllocStruct *)blocks[i].block;
            blocks[i].usedSize += sizeof(struct AllocStruct);
        }
    }
    else
    {
        // Could not find memory blocks
        core_fatal("Could not find memory blocks");
    }
}

struct BlockStruct *get_blocks(int *num)
{
    *num = numBlocks;
    return blocks;
}

struct AllocStruct *find_first_alloc(struct BlockStruct *block)
{
    if (block)
    {
        return block->first;
    }
    else
    {
        return (struct AllocStruct *)NULL;
    }
}

struct AllocStruct *find_last_alloc(struct BlockStruct *block)
{
    if (block)
    {
        return block->last;
    }
    else
    {
        return (struct AllocStruct *)NULL;
    }
}

struct AllocStruct *find_next_alloc(struct AllocStruct *allocBuffer)
{
    if (allocBuffer)
    {
        return allocBuffer->next;
    }
    else
    {
        return (struct AllocStruct *)NULL;
    }
}

struct AllocStruct *find_prev_alloc(struct AllocStruct *allocBuffer)
{
    if (allocBuffer)
    {
        return allocBuffer->prev;
    }
    else
    {
        return (struct AllocStruct *)NULL;
    }
}

// Causes memory fragmentation, use carefully
int fast_free(void *buf)
{
    struct AllocStruct *allocBuffer = buf - sizeof(struct AllocStruct);
    
    if (buf != allocBuffer->buffer)
    {
        core_fatal("Couldn't fast-free pointer, is not an alloc buffer");
        
        return ERR_CODE_RELEASE;
    }
    
    // Ok, buffer relased
    allocBuffer->hole = YES;
    allocBuffer->block->usedSize -= allocBuffer->totalSize;
    
    return 0;
}

/* clang makes implicit use of memset and memcpy to initialize and copy structs */

// void *memset(void *b, int c, size_t len)
// {
//     core_set(b, c, len);
//     return b;
// }

// void *memcpy(void *restrict dst, const void *restrict src, size_t n)
// {
//     core_copy(dst, (void *)src, n);
//     return dst;
// }
