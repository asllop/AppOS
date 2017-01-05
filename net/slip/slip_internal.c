#include <serial/serial.h>

byte slip_serial_get(PORT port)
{
    byte c;
    while (!serial_receive(port, &c, 1));
    return c;
}

void slip_serial_put(PORT port, byte c)
{
    serial_send(port, &c, 1);
}
