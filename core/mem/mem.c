#include <mem/mem_internal.h>
#include <mem/mem.h>
#include <sys/sys.h>
#include <sys/sys_internal.h>

void *core_malloc(size_t size)
{
    core_lock(MUTEX_MEM);
    
    byte numBlocks;
    struct BlockStruct *blocks = get_blocks(&numBlocks);
    
    // Find a block with enought free space
    for (byte i = 0 ; i < numBlocks ; i++)
    {
        // TODO: find a number of contiguous free segments
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
