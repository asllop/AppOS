#include <mem/mem.h>
#include <sys/sys.h>
#include <task/task.h>
#include <serial/serial.h>
#include <net/slip/slip.h>
#include <net/net.h>
#include <lib/NQCLib/NQCLib.h>

struct NetSocket sock;
byte pong[20];

void readCallback(struct NetSocket *socket, struct NetFragList packet)
{
    core_log("Received Pong: ");
    
    size_t szRead = net_read(&packet, 0, pong, 20);
    pong[szRead] = 0;
    
    // Print what we received
    core_log((char *)pong);
    
    net_free(&packet);
}

int main(int argc, char **argv)
{
    // Init pong string
    strcpy((char *)pong, "HOLA\n");
    
    // Setup serial port 0 (used by core_log)
    if (serial_init(0, SERIAL_DATA_8b, SERIAL_PARITY_NONE, SERIAL_STOP_1b, 9600))
    {
        core_fatal("Error setting up serial device 0");
    }
    
    // Setup serial port 1 (used by SLIP)
    if (serial_init(1, SERIAL_DATA_8b, SERIAL_PARITY_NONE, SERIAL_STOP_1b, 9600))
    {
        core_fatal("Error setting up serial device 1");
    }
    
    // Init SLIP in serial port 1
    NETWORK net = slip_init(1, "192.168.1.2");
    
    if (net == (NETWORK)-1)
    {
        core_fatal("Error initializing SLIP Network Interface");
    }
    
    core_log("Create sockets\n");
    
    byte destIP[] = {192,168,1,1};
    sock = net_socket(NET_SOCKET_TYPE_UDPCLIENT, destIP, 25000, 1500, 0);
    
    // Register a callback to read data comming from the socket
    if (net_open(&sock, readCallback))
    {
        core_fatal("Error opening socket");
        return -1;
    }
    
    char out[100];
    // Wait forever
    for (;;)
    {
        core_sleep(2000);
        net_send(&sock, NULL, pong, strlen((char *)pong));
        sprintf(out, "USED MEM = %u\n", core_avail(MEM_TYPE_USED));
        core_log(out);
    }
    
    return 0;
}
