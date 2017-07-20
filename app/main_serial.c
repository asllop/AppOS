#include <mem/mem.h>
#include <task/task.h>
#include <event/event.h>
#include <sys/sys.h>
#include <serial/serial.h>
#include <term/ansi.h>
#include <lib/NQCLib/NQCLib.h>
#include "utils.h"

#define ESC "\x1b["

void main(__unused int argc, __unused char **argv)
{
    char var_str[20];
    char *message = "Hello AppOS!\n";
    
    PORT port = 0;
    int line = 3;
    
    // Draw Background
    console_put_data(0x1b, 176, 80*25, 0);

	console_put_string(0x4f, " Hello AppOS ", 34, 1);

    if (serial_init(port, SERIAL_DATA_8b, SERIAL_PARITY_NONE, SERIAL_STOP_1b, 9600))
    {
        core_fatal("Error setting up serial device");
    }
    
    char cmd[10];
    
    // Clear screen
    ansi_clear_screen(cmd);
    serial_send(port, (byte *)cmd, strlen(cmd));
    
    // Set Cursor Pos
    ansi_set_cursor(cmd, 20, 10);
    serial_send(port, (byte *)cmd, strlen(cmd));
    
    // Set Gfx modes (Text and Background colors in this case)
    ansi_set_gfx_mode(cmd, (int[]){37, 41}, 2);
    serial_send(port, (byte *)cmd, strlen(cmd));
    
    serial_send(port, (byte *)message, strlen(message));
    
    // Disable gfx modes (colors)
    ansi_set_gfx_mode(cmd, (int[]){0}, 1);
    serial_send(port, (byte *)cmd, strlen(cmd));
    
    serial_send(port, (byte *)message, strlen(message));
    
    // Hide/show cursor
    ansi_show_cursor(cmd);
    serial_send(port, (byte *)cmd, strlen(cmd));
    
    // Report cursor position
    ansi_report_cursor(cmd);
    serial_send(port, (byte *)cmd, strlen(cmd));
    
    // Parse cursor report response
    int row, col;
    if (ansi_parse_report(ESC"21;56R", &row, &col))
    {
        console_put_string(0x4f, itoa(row, var_str, 10), 20, 4);
        console_put_string(0x4f, itoa(col, var_str, 10), 20, 5);
    }
    
    char data[5];
    
    core_set(data, 0, 5);
    
    for (;;)
    {
        serial_wait(port);
        console_put_string(0x4f, "> ", 0, line);
        size_t sz = serial_receive(port, (byte *)data, 5);
        data[sz] = 0;
        console_put_string(0x4f, data, 2, line++);
    }
    
	console_put_string(0x4f, "End", 2, line++);

    // When main function reaches end, the system is restarted
    core_exit(0, NULL);
}
