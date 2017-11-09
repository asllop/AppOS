#include <mem/mem.h>
#include <task/task.h>
#include <event/event.h>
#include <sys/sys.h>
#include <serial/serial.h>
#include <term/ansi.h>
#include <lib/NQCLib/NQCLib.h>
#include "utils.h"

PORT port = 0;
int cnt = 0;

void callback(PORT port)
{
    core_log("Arrived byte... ");
    
    char data[5] = {0,0,0,0,0};
    
    while (serial_avail(port))
    {
        serial_receive(port, (byte *)data, 1);
        core_log(data);
    }
    
    core_log("\n");
}

void periodic_task()
{
    while (true)
    {
        core_sleep(1500);
        core_log("Ping!\n");
        cnt ++;
        if (cnt == 5)
        {
            core_log("Disable callback\n");
            serial_callback(port, NULL);
        }
        if (cnt == 10)
        {
            core_log("Enable callback again\n");
            serial_callback(port, callback);
        }
    }
}

int main(__unused int argc, __unused char **argv)
{
    if (serial_init(port, SERIAL_DATA_8b, SERIAL_PARITY_NONE, SERIAL_STOP_1b, 9600))
    {
        core_fatal("Error setting up serial device");
    }
    
    core_log("Start pressing keys...\n\n");
    
    serial_callback(port, callback);
    
    core_create(periodic_task, 0, DEFAULT_STACK_SIZE);
    
    while (true) core_sleep(0);
    
    return 0;
}
