#include <appos.h>
#include <serial/serial.h>

#ifndef SLIP_H
#define	SLIP_H

void                        slip_init(PORT port);
void                        slip_send(char *p, int len);
int                         slip_recv(char *p, int len);

#endif