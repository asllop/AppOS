#include <sys/sys.h>
#include <task/task.h>
#include <serial/serial.h>
#include <serial/serial_internal.h>

static time_t serial_timeout_t_ms = 500;

size_t serial_send(PORT serial_port, byte *data, size_t size)
{
    // TODO: do we really need a mutex here? Try removing it
    //       As long as the port is different, we probably won't have conflicts using this functions from various threads
    
    core_lock(MUTEX_SERIAL_TX);
    
    for (size_t i = 0 ; i < size ; i++)
    {
        while (serial_is_transmit_empty(serial_port) == 0)
        {
            core_sleep(0);
        }
        
        serial_write_byte(serial_port, data[i]);
    }
    
    core_unlock(MUTEX_SERIAL_TX);
    
    return size;
}

size_t serial_receive(PORT serial_port, byte *data, size_t size)
{
    size_t read_bytes;
    TIME lastTimestamp = core_timestamp();
    
    // TODO: do we really need a mutex here? Try removing it
    //       As long as the port is different, we probably won't have conflicts using this functions from various threads
    
    core_lock(MUTEX_SERIAL_RX);
    
    for (read_bytes = 0 ; read_bytes < size ; read_bytes++)
    {
        while (serial_is_data_ready(serial_port) == 0)
        {
            core_sleep(0);
            
            if (core_timestamp() - lastTimestamp >= (TIME)serial_timeout_t_ms)
            {
                break;
            }
        }
        
        if (serial_is_data_ready(serial_port))
        {
            data[read_bytes] = serial_read_byte(serial_port);
            lastTimestamp = core_timestamp();
        }
        else
        {
            break;
        }
    }
    
    core_unlock(MUTEX_SERIAL_RX);
    
    return read_bytes;
}

bool serial_avail(PORT serial_port)
{
    return serial_is_data_ready(serial_port);
}

void serial_wait(PORT serial_port)
{
    while (serial_is_data_ready(serial_port) == 0)
    {
        core_sleep(0);
    }
}

time_t serial_timeout(time_t millis)
{
    if (millis > 0) serial_timeout_t_ms = millis;
    
    return serial_timeout_t_ms;
}
