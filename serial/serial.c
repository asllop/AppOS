#include <task/task.h>
#include <serial/serial.h>
#include <serial/serial_internal.h>

#define READ_RETRY      3

void serial_send(PORT serial_port, byte *data, size_t size)
{
    for (size_t i = 0 ; i < size ; i++)
    {
        while (is_transmit_empty(serial_port) == 0)
        {
            core_sleep(0);
        }
        
        serial_write_byte(serial_port, data[i]);
    }
}

size_t serial_receive(PORT serial_port, byte *data, size_t size)
{
    size_t read_bytes;
    
    for (read_bytes = 0 ; read_bytes < size ; read_bytes++)
    {
        int cnt = 0;
        
        while (is_data_ready(serial_port) == 0)
        {
            if (cnt >= READ_RETRY)
            {
                break;
            }
            
            core_sleep(0);
            cnt ++;
        }
        
        if (is_data_ready(serial_port))
        {
            data[read_bytes] = serial_read_byte(serial_port);
        }
        else
        {
            break;
        }
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
