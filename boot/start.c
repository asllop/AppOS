#include <appos.h>
#include <sys/sys.h>
#include <sys/sys_internal.h>
#include <task/task.h>
#include <task/task_internal.h>

int                         main(int, char **);
char                        **setup_cmdline(int *argcPointer);

static int argc;
static char **argv;

void main_task()
{
    int ret = main(argc, argv);
    
    if (ret == 0)
    {
        // End task normally
        return;
    }
    else if (ret > 0)
    {
        core_reboot();
    }
    else if (ret < 0)
    {
        core_halt();
    }
}

void kernel_init()
{
    argv = setup_cmdline(&argc);
    
    core_init();
    
    TASK mainTaskID = core_create(main_task, 0, DEFAULT_STACK_SIZE);
    
    if (!mainTaskID)
    {
        core_fatal("Could not create Main task");
    }
    
    struct TaskStruct *mainTaskStruct = task_get(mainTaskID);
    
    // Setup initial context for main task and run it
    task_setup_stack(mainTaskStruct->stack);
}
