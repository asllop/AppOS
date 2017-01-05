#include <appos.h>
#include <serial/serial.h>

#ifndef SLIP_H
#define	SLIP_H

void                        slip_send(PORT port, byte *p, int len);
int                         slip_recv(PORT port, byte *p, int len);

#endif