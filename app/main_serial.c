#include <mem/mem.h>
#include <task/task.h>
#include <event/event.h>
#include <sys/sys.h>
#include <serial/serial.h>
#include "utils.h"

void main(__unused int argc, __unused char **argv)
{
	char *hola = "Write data:\n";
    PORT port = 0;
    int line = 3;
    
    // Draw Background
    console_put_data(0x1b, 176, 80*25, 0);

	console_put_string(0x4f, " Hello AppOS ", 34, 1);

    if (serial_init(port, SERIAL_DATA_8b, SERIAL_PARITY_NONE, SERIAL_STOP_1b, 9600))
    {
        core_fatal("Error seting up serial device");
    }
    
    serial_send(port, (byte *)hola, strlen(hola));
    
    char data[5];
    
    core_set(data, 0, 5);
    
    for (;;)
    {
        serial_wait(port);
        console_put_string(0x4f, "> ", 0, line);
        size_t sz = serial_receive(port, data, 5);
        data[sz] = 0;
        console_put_string(0x4f, data, 2, line++);
    }
    
	console_put_string(0x4f, "End", 2, line++);

    // When main function reaches end, the system is restarted
    core_exit(0, NULL);
}
