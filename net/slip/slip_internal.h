#include <serial/serial.h>

#ifndef SLIP_INTERNAL_H
#define	SLIP_INTERNAL_H

byte                        slip_serial_get(PORT port);
void                        slip_serial_put(PORT port, byte c);

#endif
