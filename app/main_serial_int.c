#include <mem/mem.h>
#include <task/task.h>
#include <event/event.h>
#include <sys/sys.h>
#include <serial/serial.h>
#include <term/ansi.h>
#include <lib/NQCLib/NQCLib.h>
#include "utils.h"

PORT port = 0;

void periodic_task()
{
    while (true)
    {
        core_sleep(1500);
        core_log("Ping!\n");
    }
}

void callback()
{
    core_log("Arribat... ");
    
    char data[5] = {0,0,0,0,0};
    
    while (serial_avail(port))
    {
        serial_receive(port, (byte *)data, 1);
        core_log(data);
    }
    
    core_log("Sortim\n");
}

int main(__unused int argc, __unused char **argv)
{
    if (serial_init(port, SERIAL_DATA_8b, SERIAL_PARITY_NONE, SERIAL_STOP_1b, 9600))
    {
        core_fatal("Error setting up serial device");
    }
    
    core_log("Start Serial Callback example\n");
    
    serial_callback(port, callback);
    
    core_log("We have configured a callback\n");
    
    core_create(periodic_task, 0, DEFAULT_STACK_SIZE);
    
    for (;;);
    
    return 0;
}
