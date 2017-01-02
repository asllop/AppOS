#include <serial/serial.h>

static PORT slip_serial_port;

char slip_serial_get()
{
    char c;
    serial_receive(slip_serial_port, (byte *)&c, 1);
    return c;
}

void slip_serial_put(char c)
{
    serial_send(slip_serial_port, (byte *)&c, 1);
}

void slip_set_serial_port(PORT port)
{
    slip_serial_port = port;
}
