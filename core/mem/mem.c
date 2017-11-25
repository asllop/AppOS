#include <mem/mem_internal.h>
#include <mem/mem.h>
#include <sys/sys.h>
#include <sys/sys_internal.h>
#include <lib/NQCLib/NQCLib.h>

void *core_malloc(size_t size)
{
    size_t maxBuffSize = (size_t)((SEGMENT)-1) * SEGMENT_SIZE;
    if (size >= maxBuffSize)
    {
        // Can't allocate that buffer size, "SEGMENT" type precision is not big enought
        return NULL;
    }
    
    core_lock(MUTEX_MEM);
    
    byte numBlocks;
    struct BlockStruct *blocks = mem_get_blocks(&numBlocks);
    
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

void *core_realloc(void *buf, size_t currentBufferSize, size_t newSize, long moveOffset)
{
    core_lock(MUTEX_MEM);
    
    SEGMENT currentSegmentSize = *((SEGMENT *)(buf - sizeof(SEGMENT)));
    size_t currentSegmentPayloadSizeInBytes = ((size_t)currentSegmentSize * SEGMENT_SIZE) - sizeof(SEGMENT);
    
    if (currentSegmentPayloadSizeInBytes < newSize)
    {
        // Calculate how many additional segments we need
        size_t additionalBytesNeeded = newSize - currentSegmentPayloadSizeInBytes;
        SEGMENT additionalSegmentsNedded = additionalBytesNeeded / SEGMENT_SIZE;
        if (additionalBytesNeeded % SEGMENT_SIZE)
        {
            additionalSegmentsNedded ++;
        }
        
        void *newBuffPointer = NULL;
        
        // Find needed segments after
        void *nextSegment = (buf - sizeof(SEGMENT)) + (currentSegmentPayloadSizeInBytes + sizeof(SEGMENT));
        
        SEGMENT foundSegments = 0;
        
        while (foundSegments < additionalSegmentsNedded)
        {
            SEGMENT segSize = *((SEGMENT *)nextSegment);
            
            if (segSize == 0)
            {
                // Found free segment
                foundSegments ++;
                // Go to next segment
                nextSegment += SEGMENT_SIZE;
            }
            else
            {
                // Used segment, finish
                break;
            }
        }
        
        // Added segments after, update pointer to new segment (basically the same it was)
        newBuffPointer = buf - sizeof(SEGMENT);
        
        // NOTE: Currently we are only reallocating in one direction (forward to a higher address). The reason is:
        // PROBLEM
        // IF foundSegments < additionalSegmentsNedded => Not enought segments yet, we have to find needed segments before.
        // But we can't go back because we don't know the size of previous segments
        // Only two solutions (with current memory system)
        // 1. Start seeking from the beggining of block and go forward up to current segment
        // 2. Just realloc in one direction and ignore the other.
        // The solution 1 is too slow, so we just ignore free segments behind and scan the ones allocated in front.
        
        if (foundSegments < additionalSegmentsNedded)
        {
            // Couldn't reallocate, try to alloc a new buffer and move the data
            core_unlock(MUTEX_MEM);
            void *newbuff = core_malloc(newSize);
            
            if (newbuff)
            {
                memcpy(newbuff, buf, currentBufferSize);
                mem_move_offset(newbuff, currentBufferSize, moveOffset);
                core_free(buf);
                return newbuff;
            }
            else
            {
                // No luck, failed
                return NULL;
            }
        }
        else
        {
            // Found all segments!!
            *((SEGMENT *)newBuffPointer) += additionalSegmentsNedded;
            newBuffPointer += sizeof(SEGMENT);
            mem_move_offset(newBuffPointer, currentBufferSize, moveOffset);
            core_unlock(MUTEX_MEM);
            return newBuffPointer;
        }
    }
    else
    {
        // Current segment is big enought to alloc "size"
        mem_move_offset(buf, currentBufferSize, moveOffset);
        core_unlock(MUTEX_MEM);
        return buf;
    }
}

void core_free(void *buf)
{
    // TODO: core_free should be reentrant, we could probably get rid of the mutex here
    core_lock(MUTEX_MEM);
    
    mem_internal_free(buf);
    
    core_unlock(MUTEX_MEM);
}

size_t core_size(void *buf)
{
    SEGMENT sizeSegs = *((SEGMENT *)(buf - sizeof(SEGMENT)));
    
    if (sizeSegs > 0)
    {
        return ((size_t)sizeSegs * SEGMENT_SIZE) - sizeof(SEGMENT);
    }
    else
    {
        return 0;
    }
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
            struct BlockStruct *blocks = mem_get_blocks(&numBlocks);
            
            for (byte i = 0 ; i < numBlocks ; i++)
            {
                total += blocks[i].blockSize;
            }
            
            core_unlock(MUTEX_MEM);
            
            return total;
        }
            
        case MEM_TYPE_USED:
        {
            size_t used = 0;
            
            core_lock(MUTEX_MEM);
            
            byte numBlocks;
            struct BlockStruct *blocks = mem_get_blocks(&numBlocks);
            
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
