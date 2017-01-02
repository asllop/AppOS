#include <serial/serial.h>

#ifndef SLIP_INTERNAL_H
#define	SLIP_INTERNAL_H

char                        slip_serial_get();
void                        slip_serial_put(char c);
void                        slip_set_serial_port(PORT port);

#endif
