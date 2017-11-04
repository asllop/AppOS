#ifndef SERIAL_H
#define	SERIAL_H

#include <appos.h>

typedef enum
{
    SERIAL_DATA_8b          = 0,
    SERIAL_DATA_7b,
    SERIAL_DATA_6b,
    SERIAL_DATA_5b
    
} SERIAL_DATA;

typedef enum
{
    SERIAL_STOP_1b          = 0,
    SERIAL_STOP_2b
    
} SERIAL_STOP;

typedef enum
{
    SERIAL_PARITY_NONE      = 0,
    SERIAL_PARITY_ODD,
    SERIAL_PARITY_EVEN,
    SERIAL_PARITY_MARK,
    SERIAL_PARITY_SPACE
    
} SERIAL_PARITY;

typedef unsigned char       PORT;

size_t                      serial_send(PORT serial_port, byte *data, size_t size);
size_t                      serial_receive(PORT serial_port, byte *data, size_t size);
bool                        serial_avail(PORT serial_port);
void                        serial_wait(PORT serial_port);
time_t                      serial_timeout(time_t millis);

/* Architecture dependant functions */

int                         serial_init(PORT port, SERIAL_DATA data, SERIAL_PARITY parity, SERIAL_STOP stop, int baudrate);
void                        serial_callback(PORT port, void (*callback)());

#endif
