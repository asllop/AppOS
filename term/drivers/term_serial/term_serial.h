#include <appos.h>

#ifndef TERM_SERIAL_H
#define	TERM_SERIAL_H

#include <serial/serial.h>
#include <term/term.h>

TERM                        term_serial_init(PORT port);

#endif
