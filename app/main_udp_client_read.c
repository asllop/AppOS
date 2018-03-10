#include <mem/mem.h>
#include <sys/sys.h>
#include <task/task.h>
#include <serial/serial.h>
#include <net/slip/slip.h>
#include <net/net.h>
#include <lib/NQCLib/NQCLib.h>
#include "utils.h"

struct NetSocket sock;

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

void receiverTask()
{
    char out[50];
    
    for (;;)
    {
        // Get packet (list of fragments) and dettach from socket
        struct NetFragList fragList = net_receive(&sock, NULL);
        
        core_log("RECEIVED PACKET IN CLIENT:\n");
        
        sprintf(out, "    Local Port = %d\n", sock.localPort);
        core_log(out);
        sprintf(out, "    Num Packets = %d\n", sock.packetCount);
        core_log(out);
        
        printaPacket(&fragList);
        
        core_log("Read data from packet...\n");
        
        byte ch[2] = {0,0};
        for (int i = 0; i < 1000; i++)
        {
            size_t szRead = net_read(&fragList, i, ch , 1);
            if (szRead == 0) break;
            core_log((char *)ch);
        }
        core_log("\n\nEnd.\n");
        
        core_log("Read data from packet in packets of 10...\n");
        
        byte ch10[14];
        for (int i = 0; i < 1000; i+=13)
        {
            size_t szRead = net_read(&fragList, i, ch10 , 13);
            if (szRead == 0) break;
            ch10[szRead] = 0;
            core_log((char *)ch10);
        }
        core_log("\n\nEnd.\n");
        
        // Free memory of current fragment
        net_free(&fragList);
    }
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
    
    core_log("Start sending...\n");
    
    byte destIP[] = {192,168,1,1};
    sock = net_socket(NET_SOCKET_TYPE_UDPCLIENT, destIP, 25000, 1500, 0);
    
    struct NetSocket sock2 = net_socket(NET_SOCKET_TYPE_UDPCLIENT, destIP, 25005, 1500, 0);
    struct NetSocket sock3 = net_socket(NET_SOCKET_TYPE_UDPCLIENT, destIP, 25010, 1500, 0);
    
    net_open(&sock2);
    net_open(&sock3);
    
    if (net_open(&sock))
    {
        core_fatal("Error opening socket");
        return -1;
    }
    
    if (!core_create(receiverTask, 0, MIN_STACK_SIZE))
    {
        core_fatal("Error crating receiver task");
        return -1;
    }
    
    lprintf("Num args = %d\n", argc);
    
    for (int i = 0 ; i < argc ; i ++)
    {
        lprintf("ARG %d = %s\n", i, argv[i]);
    }
    
    // Wait forever
    while (1);
    
    return 0;
}
