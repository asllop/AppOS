#include <appos.h>
#include <sys/sys.h>
#include <sys/sys_internal.h>
#include <task/task.h>
#include <task/task_internal.h>

int                         main(int, char **);
char                        **setup_cmdline(int *argcPointer);

void main_task()
{
    int argc = 0;
    char **argv = setup_cmdline(&argc);
	main(argc, argv);
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
    
    // Setup initial context for main task and run it
    setup_stack(mainTaskStruct->stack);
}
