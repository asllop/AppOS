#include <mem/mem_internal.h>
#include <task/task_internal.h>
// #include <event/event_internal.h>
#include <sys/sys_internal.h>

void core_init()
{
    mutex_init();
    mem_init();
    io_init();
    task_init();
    // event_init();
}
