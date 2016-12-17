#include <mem/mem_internal.h>
#include <mem/mem.h>
#include <sys/sys.h>
#include <sys/sys_internal.h>

// TEST
#include <app/utils.h>

// TODO: return in a pointer the actual buffer size
void *core_malloc(size_t size)
{
    // TEST
    char var_str[10];
    core_log("--- MALLOC\n");

    core_lock(MUTEX_MEM);
    
    byte numBlocks;
    struct BlockStruct *blocks = get_blocks(&numBlocks);
    
    // Add SEGMENT size to compensate the header part
    size += sizeof(SEGMENT);
    
    SEGMENT sizeInSegments = size / SEGMENT_SIZE;
    if (size % SEGMENT_SIZE)
    {
        sizeInSegments ++;
    }
    
    core_log("Size segments = ");
    core_log(itoa(sizeInSegments, var_str, 10));
    core_log("\n");
    
    for (byte i = 0 ; i < numBlocks ; i++)
    {
        size_t numSegments = blocks[i].blockSize / SEGMENT_SIZE;
        void *p = blocks[i].block;
        
        core_log("Num segments = ");
        core_log(itoa(numSegments, var_str, 10));
        core_log("\n");
        
        SEGMENT foundSegments = 0;
        size_t j = 0;
        
        while (j < numSegments)
        {
            core_log("---> Segment index (j) = ");
            core_log(itoa(j, var_str, 10));
            core_log("\n");
            
            if (foundSegments == sizeInSegments)
            {
                // Found room enough for the buffer
                size_t firstSegment = j - sizeInSegments;
                void *b = blocks[i].block + (SEGMENT_SIZE * firstSegment);
                *((SEGMENT *)b) = (SEGMENT)sizeInSegments;
                
                core_log("First segment = ");
                core_log(itoa(firstSegment, var_str, 10));
                core_log("\n");
                core_log("Current segment (j) = ");
                core_log(itoa(j, var_str, 10));
                core_log("\n");
                
                core_unlock(MUTEX_MEM);
                
                return b + sizeof(SEGMENT);
            }
            
            if (*((SEGMENT *)p) == 0)
            {
                core_log("---> Empty segment, see next\n");
                // Empty segment
                foundSegments ++;
                j ++;
            }
            else
            {
                core_log("---> Used segment, jump to index + size\n");
                // Used segment, part of a buffer. Jump to the end of the buffer
                foundSegments = 0;
                j += *((SEGMENT *)p);
            }
            
            core_log("P pointer = 0x");
            core_log(itoa(p, var_str, 16));
            core_log("\n");
            
            p = blocks[i].block + (SEGMENT_SIZE * j);
        }

    }
    
    core_unlock(MUTEX_MEM);
    
    return NULL;
}

void *core_realloc(void *buf, size_t size)
{
    // TODO
    return NULL;
}

void core_free(void *buf)
{
    //TEST
    core_log("--- FREE\n");
    
    core_lock(MUTEX_MEM);
    
    internal_free(buf);
    
    core_unlock(MUTEX_MEM);
}

size_t core_size(void *buf)
{
    // TODO: return size of buf
    return 0;
}

size_t core_avail(MEM_TYPE memtype)
{
    switch (memtype)
    {
        default:
        case MEM_TYPE_TOTAL:
        {
            size_t total = 0;
            
            core_lock(MUTEX_MEM);
            
            // TODO
            
            core_unlock(MUTEX_MEM);
            
            return total;
        }
            break;
            
        case MEM_TYPE_FREE:
        {
            size_t free = 0;
            
            core_lock(MUTEX_MEM);
            
            // TODO
            
            core_unlock(MUTEX_MEM);
            
            return free;
        }
            break;
            
        case MEM_TYPE_USED:
        {
            size_t used = 0;
            
            core_lock(MUTEX_MEM);
            
            // TODO
            
            core_unlock(MUTEX_MEM);
            
            return used;
        }
            break;
    }
}

// If ARCH_MEM is defined, means there is an architecture dependant definition, that is supposed to speed up the process.
#ifndef ARCH_MEM
int core_copy(void *to, void *from, size_t size)
{
    for (size_t i = 0 ; i < size ; i++)
    {
        ((unsigned char *)to)[i] = ((unsigned char *)from)[i];
    }

    return 0;
}

int core_set(void *to, unsigned char value, size_t size)
{
    for (size_t i = 0 ; i < size ; i++)
    {
        ((unsigned char *)to)[i] = value;
    }
    
    return 0;
}
#endif
