#include <mem/mem_internal.h>
#include <mem/mem.h>
#include <sys/sys.h>
#include <sys/sys_internal.h>

void *core_malloc(size_t size)
{
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
    
    for (byte i = 0 ; i < numBlocks ; i++)
    {
        size_t numSegments = blocks[i].blockSize / SEGMENT_SIZE;
        void *p = blocks[i].block;
        
        SEGMENT foundSegments = 0;
        size_t j = 0;
        
        while (j < numSegments)
        {
            if (foundSegments == sizeInSegments)
            {
                // Found room enough for the buffer
                size_t firstSegment = j - sizeInSegments;
                void *b = blocks[i].block + (SEGMENT_SIZE * firstSegment);
                *((SEGMENT *)b) = (SEGMENT)sizeInSegments;
                
                core_unlock(MUTEX_MEM);
                
                return b + sizeof(SEGMENT);
            }
            
            if (*((SEGMENT *)p) == 0)
            {
                // Empty segment
                foundSegments ++;
                j ++;
            }
            else
            {
                // Used segment, part of a buffer. Jump to the end of the buffer
                foundSegments = 0;
                j += *((SEGMENT *)p);
            }
            
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
    core_lock(MUTEX_MEM);
    
    internal_free(buf);
    
    core_unlock(MUTEX_MEM);
}

size_t core_size(void *buf)
{
    SEGMENT sizeSegs = *((SEGMENT *)(buf - sizeof(SEGMENT)));
    return ((size_t)sizeSegs * SEGMENT_SIZE) - sizeof(SEGMENT);
}

size_t core_avail(MEM_TYPE memtype)
{
    switch (memtype)
    {
        case MEM_TYPE_TOTAL:
        {
            size_t total = 0;
            
            core_lock(MUTEX_MEM);
            
            byte numBlocks;
            struct BlockStruct *blocks = get_blocks(&numBlocks);
            
            for (byte i = 0 ; i < numBlocks ; i++)
            {
                total += blocks[i].blockSize;
            }
            
            core_unlock(MUTEX_MEM);
            
            return total;
        }
            
        case MEM_TYPE_FREE:
        {
            size_t free = 0;
            
            core_lock(MUTEX_MEM);
            
            byte numBlocks;
            struct BlockStruct *blocks = get_blocks(&numBlocks);
            
            for (byte i = 0 ; i < numBlocks ; i++)
            {
                size_t numSegments = blocks[i].blockSize / SEGMENT_SIZE;
                void *p = blocks[i].block;
                size_t j = 0;
                
                while (j < numSegments)
                {
                    if (*((SEGMENT *)p) == 0)
                    {
                        // Empty segment
                        j ++;
                        free += SEGMENT_SIZE;
                    }
                    else
                    {
                        // Used segment, part of a buffer. Jump to the end of the buffer
                        j += *((SEGMENT *)p);
                    }
                    
                    p = blocks[i].block + (SEGMENT_SIZE * j);
                }
            }
            
            core_unlock(MUTEX_MEM);
            
            return free;
        }
            
        case MEM_TYPE_USED:
        {
            size_t used = 0;
            
            core_lock(MUTEX_MEM);
            
            byte numBlocks;
            struct BlockStruct *blocks = get_blocks(&numBlocks);
            
            for (byte i = 0 ; i < numBlocks ; i++)
            {
                size_t numSegments = blocks[i].blockSize / SEGMENT_SIZE;
                void *p = blocks[i].block;
                size_t j = 0;
                
                while (j < numSegments)
                {
                    SEGMENT sizeSegs = *((SEGMENT *)p);
                    if (sizeSegs == 0)
                    {
                        // Empty segment
                        j ++;
                    }
                    else
                    {
                        // Used segment, part of a buffer. Jump to the end of the buffer
                        j += sizeSegs;
                        used += SEGMENT_SIZE * sizeSegs;
                    }
                    
                    p = blocks[i].block + (SEGMENT_SIZE * j);
                }
            }
            
            core_unlock(MUTEX_MEM);
            
            return used;
        }
            
        default:
            return 0;
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
