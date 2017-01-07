#include <mem/mem.h>
#include <sys/sys.h>
#include <task/task.h>
#include <serial/serial.h>
#include <net/slip/slip.h>
#include <term/term.h>
#include <term/drivers/term_serial/term_serial.h>
#include <net/ipv4/ipv4.h>

#include "utils.h"

void main(__unused int argc, __unused char **argv)
{
	char var_str[20];
    
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
    
    // Init TERM in serial port 0
    TERM termID = term_serial_init(0);
    // Init SLIP in serial port 1
    PORT slip = 1;
    
    struct NetIfaceStruct *slipIface = slip_init(slip);
    
    if (termID == -1)
    {
        core_fatal("Term serial driver failed to init");
    }
    
    if (!slipIface)
    {
        core_fatal("Error initializing SLIP Network Interface");
    }
    
    byte *buff = (byte *)core_malloc(10000);
    
    term_puts(termID, "Waiting packets...\n");
    
    while (1)
    {
        int sz = slip_recv(slip, buff, 10000);
        
        // TODO: print data received
        
        term_puts(termID, "Received packet of size ");
        term_puts(termID, itoa(sz, var_str, 10));
        term_puts(termID, "\n");
        
        int res = ipv4_insert(slipIface, buff, sz);
        
        if (res < 0)
        {
            term_puts(termID, "Error = ");
            term_puts(termID, itoa(res, var_str, 10));
            term_puts(termID, "\n");
        }
        else
        {
            term_puts(termID, "Correct IP packet\n");
        }
    }
}