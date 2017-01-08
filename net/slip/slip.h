#ifndef SLIP_H
#define	SLIP_H

#include <appos.h>
#include <serial/serial.h>
#include <net/net.h>

NETWORK                     slip_init(PORT port);
void                        slip_send(PORT port, byte *p, int len);
int                         slip_recv(PORT port, byte *p, int len);

#endif