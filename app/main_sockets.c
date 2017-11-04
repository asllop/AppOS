#include <mem/mem.h>
#include <sys/sys.h>
#include <task/task.h>
#include <serial/serial.h>
#include <net/slip/slip.h>
#include <term/term.h>
#include <term/drivers/term_serial/term_serial.h>
#include <net/net.h>
#include <lib/NQCLib/NQCLib.h>
#include "utils.h"

int main(__unused int argc, __unused char **argv)
{
    // Draw Background
    console_put_data(0x1b, (char)176, 80*25, 0);
	console_put_string(0x4f, " NET SOCKETS ", 34, 1);
    
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
    TERM term = term_serial_init(0);
    
    // Init SLIP in serial port 1
    NETWORK net = slip_init(1, "192.168.1.2");
    
    if (term == -1)
    {
        core_fatal("Term serial driver failed to init");
    }
    
    if (net == (NETWORK)-1)
    {
        core_fatal("Error initializing SLIP Network Interface");
    }
    
    core_log("Start sending...\n");
    
    byte destIP[] = {192,168,1,1};
    struct NetSocket socket = net_socket(NET_SOCKET_TYPE_UDPCLIENT, destIP, 25000, 1500, 0);
    
    if (net_open(&socket))
    {
        core_fatal("Error opening socket");
        return -1;
    }
    
    while (1)
    {
        core_sleep(1000);
        
        char *dataToSend = core_malloc(50);
        
        TIME ts = core_timestamp();
        ltoa(ts, dataToSend, 10);
        
        size_t dataLen = strlen(dataToSend);
        
        net_send(&socket, NULL, (byte *)dataToSend, dataLen);
        
        core_log("Timestamp sent: ");
        core_log(ltoa(ts, dataToSend, 10));
        core_log("\n");
        
        core_log("Used mem: ");
        core_log(ltoa(core_avail(MEM_TYPE_USED), dataToSend, 10));
        core_log("\n");
        
        core_free(dataToSend);
    }
    
    return 0;
}
