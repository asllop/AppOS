#include <mem/mem.h>
#include <sys/sys.h>
#include <task/task.h>
#include <serial/serial.h>
#include <net/slip/slip.h>
#include <term/term.h>
#include <term/drivers/term_serial/term_serial.h>

#include "utils.h"

void main(__unused int argc, __unused char **argv)
{
	char var_str[20];
    TERM termID;
    
    // Draw Background
    console_put_data(0x1b, 176, 80*25, 0);
	console_put_string(0x4f, " NET TEST ", 34, 1);
    
    // Setup serial ports 0 and 1
    
    if (serial_init(0, SERIAL_DATA_8b, SERIAL_PARITY_NONE, SERIAL_STOP_1b, 9600))
    {
        core_fatal("Error setting up serial device 0");
    }
    
    if (serial_init(1, SERIAL_DATA_8b, SERIAL_PARITY_NONE, SERIAL_STOP_1b, 9600))
    {
        core_fatal("Error setting up serial device 1");
    }
    
    // Init SLIP in serial port 1
    slip_init(1);
    // Init TERM in serial port 0
    termID = term_serial_init(0);
    
    if (termID == -1)
    {
        core_fatal("Term serial driver failed to init");
    }
    
    char *buff = core_malloc(10000);
    
    term_puts(termID, "Hola term\n");
    
    while (1)
    {
        slip_send("Hola slip", 9);
        core_sleep(1000);
    }
    
    /*
    while (1)
    {
        int sz = slip_recv(buff, 10000);
        // TODO: print data received
    }
     */
}