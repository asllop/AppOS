#include <sys/sys_internal.h>
#include <sys/sys.h>

void core_fatal(char *msg)
{
    emergency_puts(msg);
#if REBOOT_ON_FATAL
    core_reboot();
#else
    core_halt();
#endif
}