#include <appos.h>
#include <serial/serial.h>

#ifndef SERIAL_INTERNAL_H
#define SERIAL_INTERNAL_H

/* Architecture dependant functions */

byte                        is_data_ready(PORT port);
byte                        is_transmit_empty(PORT port);
void                        serial_write_byte(PORT port, byte dat);
byte                        serial_read_byte(PORT port);

#endif
