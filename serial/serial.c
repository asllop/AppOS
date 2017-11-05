#include <sys/sys.h>
#include <task/task.h>
#include <serial/serial.h>
#include <serial/serial_internal.h>

static time_t serial_timeout_t_ms = 500;

/*
 TODO:
 
 Generalize Serial API: to make the serial_ module work with other hw interfaces like SPI and I2C.
 
 Create a set of serial_config_XYZ, where XYZ is: UART, SPI, I2C. Each of thos functions take the necessary arguments for the specific
 port. Example:
 
 serial_config_uart(PORT port, SERIAL_DATA data, SERIAL_PARITY parity, SERIAL_STOP stop, int baudrate)
 
 Those functions will return a SerialConfigStruct representing a configured port. This struct will be passed as argument to serial_init,
 to actually initialize the port.
 The rest of functions (serial_send, serial_receive, serial_avail, serial_wait) will use this struct as arguments instead of PORT.
 
 */

size_t serial_send(PORT serial_port, byte *data, size_t size)
{
    core_lock(MUTEX_SERIAL);
    
    for (size_t i = 0 ; i < size ; i++)
    {
        while (serial_is_transmit_empty(serial_port) == 0)
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
    
    core_unlock(MUTEX_SERIAL);
    
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
    if (millis > 0) {
        serial_timeout_t_ms = millis;
    }
    return serial_timeout_t_ms;
}
