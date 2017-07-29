#ifndef SERIAL_INTERNAL_H
#define SERIAL_INTERNAL_H

#include <appos.h>
#include <serial/serial.h>

/* Architecture dependant functions */

byte                        serial_is_data_ready(PORT port);
byte                        serial_is_transmit_empty(PORT port);
void                        serial_write_byte(PORT port, byte dat);
byte                        serial_read_byte(PORT port);

#endif
