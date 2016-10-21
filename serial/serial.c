#include <task/task.h>
#include <serial/serial.h>
#include <serial/serial_internal.h>

#define READ_RETRY      3

size_t serial_send(PORT serial_port, byte *data, size_t size)
{
    for (size_t i = 0 ; i < size ; i++)
    {
        while (is_transmit_empty(serial_port) == 0)
        {
            core_sleep(0);
        }
        
        serial_write_byte(serial_port, data[i]);
    }
    
    return size;
}

size_t serial_receive(PORT serial_port, byte *data, size_t size)
{
    size_t read_bytes;
    
    for (read_bytes = 0 ; read_bytes < size ; read_bytes++)
    {
        while (is_data_ready(serial_port) == 0)
        {
            core_sleep(0);
        }
        
        data[read_bytes] = serial_read_byte(serial_port);
    }
    
    return read_bytes;
}

bool serial_avail(PORT serial_port)
{
    return is_data_ready(serial_port);
}

void serial_wait(PORT serial_port)
{
    while (is_data_ready(serial_port) == 0)
    {
        core_sleep(0);
    }
}
