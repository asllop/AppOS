#include <mem/mem.h>
#include <sys/sys.h>
#include <task/task.h>
#include <serial/serial.h>
#include <net/slip/slip.h>
#include <net/net.h>
#include <lib/NQCLib/NQCLib.h>

byte buf[20];

// TODO: get client information from socket (not filled yet) and suport multiple clients.
// TODO: get client IP i send back a response (echo)

void readCallback(struct NetSocket *socket, struct NetFragList packet)
{
    core_log("Server Received Data: ");
    
    size_t szRead = net_read(&packet, 0, buf, 20);
    buf[szRead] = 0;
    
    // Print what we received
    core_log((char *)buf);
    
    net_free(&packet);
}

int main(int argc, char **argv)
{
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
    
    byte localIP[] = {192,168,1,2};
    struct NetSocket sock = net_socket(NET_SOCKET_TYPE_UDPSERVER, localIP, 1500, 0, 0);
    
    // Register a callback to read data comming from the socket
    if (net_open(&sock, readCallback))
    {
        core_fatal("Error opening socket");
        return -1;
    }
    
    for (;;) core_sleep(0);
}
