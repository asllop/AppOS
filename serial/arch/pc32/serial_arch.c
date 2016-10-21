#include <appos.h>
#include <sys/io/arch/pc32/io.h>
#include <serial/serial.h>

unsigned int port_address(PORT port)
{
    switch (port)
    {
        default:
        case 0:
            return 0x3F8;
        case 1:
            return 0x2F8;
        case 2:
            return 0x3E8;
        case 3:
            return 0x2E8;
    }
}

unsigned char port_databits(SERIAL_DATA data)
{
    switch (data)
    {
        default:
        case SERIAL_DATA_8b:
            return 3;
        case SERIAL_DATA_7b:
            return 2;
        case SERIAL_DATA_6b:
            return 1;
        case SERIAL_DATA_5b:
            return 0;
    }
}

unsigned char port_stopbits(SERIAL_STOP stop)
{
    switch (stop)
    {
        default:
        case SERIAL_STOP_1b:
            return 0;
        case SERIAL_STOP_2b:
            return 4;
    }
}

unsigned char port_parity(SERIAL_PARITY parity)
{
    switch (parity)
    {
        default:
        case SERIAL_PARITY_NONE:
            return 0;
        case SERIAL_PARITY_ODD:
            return 8;
        case SERIAL_PARITY_EVEN:
            return 24;
        case SERIAL_PARITY_MARK:
            return 40;
        case SERIAL_PARITY_SPACE:
            return 56;
    }
}

// TODO: check interrupts stuff

int serial_setup(PORT port, SERIAL_DATA data, SERIAL_PARITY parity, SERIAL_STOP stop, int baudrate)
{
    if (115200 % baudrate != 0 || baudrate <= 0)
    {
        return ERR_CODE_BADSERIALCONFIG;
    }
    
    uint16_t speed_ticks = 115200 / baudrate;
    
    unsigned int port_addr = port_address(port);
    
    outportb(port_addr + 1, 0x00);                          // Disable all interrupts
    outportb(port_addr + 3, 0x80);                          // Enable DLAB (set baud rate divisor)
    outportb(port_addr + 0, speed_ticks & 0xFF);            // Set clock divisor (LO byte)
    outportb(port_addr + 1, (speed_ticks >> 8) & 0xFF);     // Set clock divisor (HI byte)
    
    unsigned char port_config = port_databits(data) | port_stopbits(stop) | port_parity(parity);
    outportb(port_addr + 3, port_config);                   // Data bits, stop bits, parity
    
    outportb(port_addr + 2, 0xC7);                          // Enable FIFO, clear them, with 14-byte threshold
    outportb(port_addr + 4, 0x0B);                          // IRQs enabled, RTS/DSR set
    
    return 0;
}

byte is_data_ready(PORT port)
{
    return inportb(port_address(port) + 5) & 1;
}

byte is_transmit_empty(PORT port)
{
    return inportb(port_address(port) + 5) & 0x20;
}

void serial_write_byte(PORT port, byte dat)
{
    outportb(port_address(port), dat);
}

byte serial_read_byte(PORT port)
{
    return inportb(port_address(port));
}
