#include <common/appos.h>
#include <sys/sys.h>
#include <sys/sys_internal.h>
#include <task/task.h>
#include <task/task_internal.h>

int                         main(int, char **);

void main_task()
{
	main(0, NULL);
	core_reboot();
}

void kernel_init()
{
    core_init();
    
    TASK mainTaskID = core_create(main_task, 0, DEFAULT_STACK_SIZE);
    
    if (!mainTaskID)
    {
        core_fatal("Could not create Main task");
    }
    
    struct TaskStruct *mainTaskStruct = get_task(mainTaskID);
    mainTaskStruct->state = TASK_STATE_STARTING;
    
    // Task Scheduling is disabled by default, so we need to enable it and start the multitasking magic!
    core_permit();

    for(;;);
}
