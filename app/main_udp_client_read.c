#include <mem/mem.h>
#include <sys/sys.h>
#include <task/task.h>
#include <serial/serial.h>
#include <net/slip/slip.h>
#include <net/net.h>
#include <lib/NQCLib/NQCLib.h>
#include "utils.h"

struct NetSocket sock, sock2, sock3;

int lprintf(const char *fmt, ...)
{
    va_list args;
    int i;
    
    void *buf = core_malloc(500);
    if (!buf) return 0;
    
    va_start(args, fmt);
    i = vsprintf(buf, fmt, args);
    va_end(args);
    
    core_log(buf);
    
    core_free(buf);
    
    return i;
}

void printaPacket(struct NetFragList *fragList)
{
    size_t len = net_size(fragList);
    struct NetFrag *nextBuff = fragList->first;
    
    for (int i = 0 ; i < fragList->numFragments ; i ++)
    {
        if (nextBuff)
        {
            char out[50];
            
            core_log("------- PRINT FRAGMENT --------\n");
            sprintf(out, "--- Payload Size = %d ---\n", len);
            core_log(out);
            
            for (int i = 0 ; i < nextBuff->size ; i++)
            {
                sprintf(out, "%x ", nextBuff->packet[i]);
                core_log(out);
            }
            core_log("\n------- END PRINT FRAGMENT --------\n");
            
            nextBuff = nextBuff->next;
        }
        else
        {
            break;
        }
    }
}

void readCallback(struct NetSocket *socket, struct NetFragList packet)
{
    char out[50];
    
    core_log("RECEIVED PACKET IN CLIENT:\n");
    sprintf(out, "    Local Port = %d\n", socket->localPort); core_log(out);
    sprintf(out, "    Remote Port = %d\n", socket->remotePort); core_log(out);
    sprintf(out, "    Address = %d.%d.%d.%d\n", socket->address[0],socket->address[1],socket->address[2],socket->address[3]); core_log(out);
    
    core_log("Data received...\n");
    
    byte buff[100];
    int offset = 0;
    for (;;)
    {
        size_t szRead = net_read(&packet, offset, buff , 100);
        if (szRead == 0) break;
        offset += szRead;
        buff[szRead] = 0;
        core_log((char *)buff);
    }

    core_log("End.\n");
    
    net_free(&packet);
    
    core_log("After net free\n");
}

int main(int argc, char **argv)
{
    // Draw Background
    console_put_data(0x1b, (char)176, 80*25, 0);
	console_put_string(0x4f, " NET SOCKETS ", 34, 1);
    
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
    sock2 = net_socket(NET_SOCKET_TYPE_UDPCLIENT, destIP, 25005, 1500, 0);
    sock3 = net_socket(NET_SOCKET_TYPE_UDPCLIENT, destIP, 25010, 1500, 0);
    
    core_log("Open sockets\n");
    
    // TODO:
    /*
     - Implementar fragmentació de sortida
     - Implementar servidors
     - Implementar RAW sockets (client i servidor)
     */
    
    if (net_open(&sock, readCallback))
    {
        core_fatal("Error opening socket");
        return -1;
    }
    
    if (net_open(&sock2, readCallback))
    {
        core_fatal("Error opening socket 2");
        return -1;
    }
    
    if (net_open(&sock3, readCallback))
    {
        core_fatal("Error opening socket 3");
        return -1;
    }
    
    lprintf("Num args = %d\n", argc);
    
    for (int i = 0 ; i < argc ; i ++)
    {
        lprintf("ARG %d = %s\n", i, argv[i]);
    }
    
    // Wait forever
    while (1)
    {
        core_sleep(0);
    }
    
    return 0;
}
