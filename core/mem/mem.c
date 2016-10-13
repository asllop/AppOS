#include <mem/mem_internal.h>
#include <mem/mem.h>
#include <sys/sys.h>
#include <sys/sys_internal.h>

#define MARGIN              4 * sizeof(struct AllocStruct)
#define BUFFER_MIN_SIZE(X)  X + sizeof(struct AllocStruct) + MARGIN

void *core_malloc(size_t size)
{
    acquire_mutex(MUTEX_MEM);
    
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
                        
                        free_mutex(MUTEX_MEM);
                        
                        return allocBuffer->buffer;
                    }
                }
                
                allocBuffer = find_next_alloc(allocBuffer);
            }
        }
    }
    
    free_mutex(MUTEX_MEM);
    
    return NULL;
}

void *core_realloc(void *buf, size_t size)
{
    void *newBuff = core_malloc(size);
    
    if (!newBuff)
    {
        return NULL;
    }
    
    acquire_mutex(MUTEX_MEM);
    
    struct AllocStruct *allocBuffer = buf - sizeof(struct AllocStruct);
    
    if (buf != allocBuffer->buffer)
    {
        core_fatal("Couldn't realloc, pointer is not an alloc buffer");
    }
    
    size_t oldSize = allocBuffer->totalSize;
    
    size_t copySize;
    
    if (oldSize < size)
    {
        copySize = oldSize;
    }
    else
    {
        copySize = size;
    }
    
    free_mutex(MUTEX_MEM);
    
    core_copy(newBuff, buf, copySize);
    core_free(buf);
    
    return newBuff;
}

int core_free(void *buf)
{
    if (!buf)
    {
        return ERR_CODE_RELEASE;
    }
    
    acquire_mutex(MUTEX_MEM);
    
    int ret = fast_free(buf);
    
//    struct AllocStruct *allocBuffer = buf - sizeof(struct AllocStruct);
//    
//    if (buf != allocBuffer->buffer)
//    {
//        core_fatal("Couldn't free pointer, is not an alloc buffer");
//    }
//    
//    // Ok, buffer relased, now let's try to join boundary buffers
//    allocBuffer->hole = YES;
//    allocBuffer->block->usedSize -= allocBuffer->totalSize;
//    
//    // Join contiguous buffers, Prev
//    
//    struct AllocStruct *prevBuffer = find_prev_alloc(allocBuffer);
//    
//    while (prevBuffer)
//    {
//        if (prevBuffer->hole == YES)
//        {
//            prevBuffer->next = allocBuffer->next;
//            
//            if (allocBuffer->next)
//            {
//                allocBuffer->next->prev = prevBuffer;
//            }
//            
//            prevBuffer->totalSize += allocBuffer->totalSize + sizeof(struct AllocStruct);
//            prevBuffer->block->usedSize -= sizeof(struct AllocStruct);
//            allocBuffer->buffer = NULL;
//            
//            allocBuffer = prevBuffer;
//        }
//        else
//        {
//            // No more free space to join
//            break;
//        }
//        
//        prevBuffer = find_prev_alloc(allocBuffer);
//    }
//    
//    // Join contiguous buffers, Next (find nextBuffer starting from current allocBuffer)
//    
//    struct AllocStruct *nextBuffer = find_next_alloc(allocBuffer);
//    
//    while (nextBuffer)
//    {
//        if (nextBuffer->hole == YES)
//        {
//            allocBuffer->next = nextBuffer->next;
//            
//            if (allocBuffer->next)
//            {
//                allocBuffer->next->prev = allocBuffer;
//            }
//            
//            allocBuffer->totalSize += nextBuffer->totalSize + sizeof(struct AllocStruct);
//            allocBuffer->block->usedSize -= sizeof(struct AllocStruct);
//            nextBuffer->buffer = NULL;
//        }
//        else
//        {
//            // No more free space to join
//            break;
//        }
//        
//        nextBuffer = find_next_alloc(nextBuffer);
//    }
//    
    free_mutex(MUTEX_MEM);
    
    return ret;
    //return 0;
}

// TODO: core_defrag: join blocks to defrag memory

size_t core_avail(MEM_TYPE memtype)
{
    acquire_mutex(MUTEX_MEM);
    
    int numBlocks;
    struct BlockStruct *blocks = get_blocks(&numBlocks);
    
    switch (memtype)
    {
        case MEM_TYPE_TOTAL:
        {
            size_t total = 0;
            
            for (int i = 0 ; i < numBlocks ; i++)
            {
                total += blocks[i].totalSize;
            }
            
            free_mutex(MUTEX_MEM);
            
            return total;
        }
            break;
            
        case MEM_TYPE_FREE:
        {
            size_t free = 0;
            
            for (int i = 0 ; i < numBlocks ; i++)
            {
                free += (blocks[i].totalSize - blocks[i].usedSize);
            }
            
            free_mutex(MUTEX_MEM);
            
            return free;
        }
            break;
            
        case MEM_TYPE_USED:
        {
            size_t used = 0;
            
            for (int i = 0 ; i < numBlocks ; i++)
            {
                used += blocks[i].usedSize;
            }
            
            free_mutex(MUTEX_MEM);
            
            return used;
        }
            break;
            
        default:
            free_mutex(MUTEX_MEM);
            
            return 0;
    }
}

size_t core_best(size_t size)
{
    acquire_mutex(MUTEX_MEM);
    
    size_t biggestSize = 0;
    int numBlocks;
    struct BlockStruct *blocks = get_blocks(&numBlocks);
    
    for (int i = 0 ; i < numBlocks ; i++)
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
                    if (allocBuffer->totalSize > BUFFER_MIN_SIZE(size))
                    {
                        free_mutex(MUTEX_MEM);
                        
                        return size;
                    }
                    else
                    {
                        free_mutex(MUTEX_MEM);
                        
                        return allocBuffer->totalSize;
                    }
                }

                if (biggestSize < allocBuffer->totalSize)
                {
                    biggestSize = allocBuffer->totalSize;
                }
            }

            allocBuffer = find_next_alloc(allocBuffer);
        }
    }
    
    // Not enought space to fit the buffer, return the biggest allocable size
    free_mutex(MUTEX_MEM);
    
    return biggestSize;
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
