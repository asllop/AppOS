#ifndef MEM_H
#define	MEM_H

#include <appos.h>

typedef enum
{
    MEM_TYPE_TOTAL          = 0,
    MEM_TYPE_FREE,
    MEM_TYPE_USED,

} MEM_TYPE;

void                        *core_malloc(size_t size);
void                        *core_realloc(void *buf, size_t size);
void                        core_free(void *buf);
size_t                      core_size(void *buf);
size_t                      core_avail(MEM_TYPE memtype);

#endif
