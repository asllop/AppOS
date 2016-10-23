#include <mem/mem.h>
#include <task/task.h>
#include <event/event.h>
#include <sys/sys.h>
#include <serial/serial.h>
#include "utils.h"

#define ESC "\x1b["

void main(__unused int argc, __unused char **argv)
{
	char *setColors = ESC"37;41m";
    char *clearScreen = ESC"2J";
    char *setCursosOrg = ESC"0;0H";
    char *message = "Hello AppOS!\n\r";
    char *resetColors = ESC"0m";
    
    PORT port = 0;
    int line = 3;
    
    // Draw Background
    console_put_data(0x1b, 176, 80*25, 0);

	console_put_string(0x4f, " Hello AppOS ", 34, 1);

    if (serial_init(port, SERIAL_DATA_8b, SERIAL_PARITY_NONE, SERIAL_STOP_1b, 9600))
    {
        core_fatal("Error setting up serial device");
    }
    
    serial_send(port, (byte *)clearScreen, strlen(clearScreen));
    serial_send(port, (byte *)setCursosOrg, strlen(setCursosOrg));
    serial_send(port, (byte *)setColors, strlen(setColors));
    serial_send(port, (byte *)message, strlen(message));
    serial_send(port, (byte *)resetColors, strlen(resetColors));
    serial_send(port, (byte *)message, strlen(message));
    
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
