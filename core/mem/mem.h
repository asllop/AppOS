#include <appos.h>

#ifndef MEM_H
#define	MEM_H

typedef enum
{
    MEM_TYPE_TOTAL          = 0,
    MEM_TYPE_FREE,
    MEM_TYPE_USED,

} MEM_TYPE;

void                        *core_malloc(size_t size);
void                        *core_realloc(void *buf, size_t size);
int                         core_free(void *buf);
size_t                      core_avail(MEM_TYPE memtype);
size_t                      core_best(size_t size);
int                         core_copy(void *to, void *from, size_t size);
int                         core_set(void *to, unsigned char value, size_t size);

#endif
