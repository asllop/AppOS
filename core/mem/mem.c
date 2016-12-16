#include <mem/mem_internal.h>
#include <mem/mem.h>
#include <sys/sys.h>
#include <sys/sys_internal.h>

#define MARGIN              4 * sizeof(struct AllocStruct)
#define BUFFER_MIN_SIZE(X)  X + sizeof(struct AllocStruct) + MARGIN

void *core_malloc(size_t size)
{
    core_lock(MUTEX_MEM);
    
    int numBlocks;
    struct BlockStruct *blocks = get_blocks(&numBlocks);
    
    // Find a block with enought free space
    for (int i = 0 ; i < numBlocks ; i++)
    {
        if (blocks[i].totalSize - blocks[i].usedSize >= size)
        {
            // Find a free alloc inside the block
            struct AllocStruct *allocBuffer = find_first_alloc(&blocks[i]);
            
            while (allocBuffer)
            {
                // Found free space
                if (allocBuffer->hole == YES)
                {
                    // With size enought
                    if (allocBuffer->totalSize >= size)
                    {
                        // Enought space to split the buffer?
                        if (allocBuffer->totalSize > BUFFER_MIN_SIZE(size))
                        {
                            size_t originalSize = allocBuffer->totalSize;
                            struct AllocStruct *originalNext = allocBuffer->next;
                            
                            // Calculate pointer for the splitted buffer
                            void *nextAllocStruct = allocBuffer->buffer + size;
                            void *nextAllocBuffer = nextAllocStruct + sizeof(struct AllocStruct);
                            
                            // Transform the buffer to fit exactly the memory we need
                            allocBuffer->hole = NO;
                            allocBuffer->totalSize = size;
                            allocBuffer->next = (struct AllocStruct *)nextAllocStruct;
                            
                            // Splitted buffer struct
                            struct AllocStruct *nextStruct = (struct AllocStruct *)nextAllocStruct;
                            
                            nextStruct->block = allocBuffer->block;
                            nextStruct->buffer = nextAllocBuffer;
                            nextStruct->hole = YES;
                            nextStruct->prev = allocBuffer;
                            nextStruct->next = originalNext;
                            nextStruct->totalSize = originalSize - size - sizeof(struct AllocStruct);
                            
                            // Update used memory
                            blocks[i].usedSize += size + sizeof(struct AllocStruct);
                        }
                        else
                        {
                            // Use the current buffer as is
                            allocBuffer->hole = NO;
                            
                            // Update used memory
                            blocks[i].usedSize += size;
                        }
                        
                        core_unlock(MUTEX_MEM);
                        
                        return allocBuffer->buffer;
                    }
                }
                
                allocBuffer = find_next_alloc(allocBuffer);
            }
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

int core_free(void *buf)
{
    if (!buf)
    {
        return ERR_CODE_RELEASE;
    }
    
    core_lock(MUTEX_MEM);
    
    int ret = fast_free(buf);
    
    core_unlock(MUTEX_MEM);
    
    return ret;
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
