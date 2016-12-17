#include <mem/mem_internal.h>
#include <mem/mem.h>
#include <sys/sys.h>
#include <sys/sys_internal.h>

// TEST
#include <app/utils.h>

static struct BlockStruct   blocks[MAX_NUM_BLOCKS];
static byte                 numBlocks;

void mem_init()
{
    setup_mem();
    
    if ((numBlocks = scan_blocks(blocks)) > 0)
    {
        // Init headers for all segments (free segment)
        
        for (byte i = 0 ; i < numBlocks ; i ++)
        {
            size_t numSegments = blocks[i].blockSize / SEGMENT_SIZE;
            void *p = blocks[i].block;
            
            for (size_t j = 0 ; j < numSegments ; j ++)
            {
                *((SEGMENT *)p) = (SEGMENT)0;
                p += SEGMENT_SIZE;
            }
        }
    }
    else
    {
        // Could not find memory blocks
        core_fatal("Could not find memory blocks");
    }
}

// TEST
void check_segments()
{
    char var_str[10];
    
    for (byte i = 0 ; i < numBlocks ; i ++)
    {
        size_t numSegments = blocks[i].blockSize / SEGMENT_SIZE;
        void *p = blocks[i].block;
        
        for (size_t j = 0 ; j < numSegments ; j ++)
        {
            if (*((SEGMENT *)p) != 0)
            {
                core_log("FOUND SEGMENT HEADER NOT ZERO AT numSeg = ");
                core_log(itoa(j, var_str, 10));
                core_log(", address = 0x");
                core_log(itoa(p, var_str, 16));
                core_log(", header value = ");
                core_log(itoa(*((SEGMENT *)p), var_str, 10));
                core_log("\n");
            }
            
            p += SEGMENT_SIZE;
        }
    }
    
    core_log("FINISHED CHECKING SEGMENTS\n");
}

struct BlockStruct *get_blocks(byte *num)
{
    *num = numBlocks;
    return blocks;
}

void internal_free(void *buf)
{
    if (!buf)
    {
        return;
    }
    
    // TODO
}
