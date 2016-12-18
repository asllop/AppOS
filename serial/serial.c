#include <sys/sys.h>
#include <task/task.h>
#include <serial/serial.h>
#include <serial/serial_internal.h>

static time_t serial_timeout_t_ms = 500;

size_t serial_send(PORT serial_port, byte *data, size_t size)
{
    core_lock(MUTEX_SERIAL);
    
    for (size_t i = 0 ; i < size ; i++)
    {
        while (is_transmit_empty(serial_port) == 0)
        {
            core_sleep(0);
        }
        
        serial_write_byte(serial_port, data[i]);
    }
    
    core_unlock(MUTEX_SERIAL);
    
    return size;
}

size_t serial_receive(PORT serial_port, byte *data, size_t size)
{
    size_t read_bytes;
    TIME lastTimestamp = core_timestamp();
    
    core_lock(MUTEX_SERIAL);
    
    for (read_bytes = 0 ; read_bytes < size ; read_bytes++)
    {
        while (is_data_ready(serial_port) == 0)
        {
            core_sleep(0);
            
            if (core_timestamp() - lastTimestamp >= (TIME)serial_timeout_t_ms)
            {
                break;
            }
        }
        
        if (is_data_ready(serial_port))
        {
            data[read_bytes] = serial_read_byte(serial_port);
            lastTimestamp = core_timestamp();
        }
        else
        {
            break;
        }
    }
    
    core_unlock(MUTEX_SERIAL);
    
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

time_t serial_timeout(time_t millis)
{
    if (millis > 0) {
        serial_timeout_t_ms = millis;
    }
    return serial_timeout_t_ms;
}
