#include <appos.h>
#include <sys/io/arch/pc32/io.h>
#include <mem/arch/pc32/mem_arch.h>
#include <serial/serial.h>

extern void serial0_isr();
extern void serial1_isr();

static void (*serial_rx_callback)(PORT);

void serial_rx_isr(PORT port)
{
    // Disable NMI
    outportb(0x70, inportb(0x70) | 0x80);
    
    if (serial_rx_callback) serial_rx_callback(port);
    
    // Enable NMI
    outportb(0x70, inportb(0x70) & 0x7F);
    
    // End of PIC1 interrupt (enable for next interrupt)
    outportb(PIC1, 0x20);
}

void serial0_rx_isr()
{
    serial_rx_isr(0);
}

void serial1_rx_isr()
{
    serial_rx_isr(1);
}

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

int serial_init(PORT port, SERIAL_DATA data, SERIAL_PARITY parity, SERIAL_STOP stop, int baudrate)
{
    serial_rx_callback = NULL;
    
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

void serial_callback(PORT port, void (*callback)(PORT))
{
    if (callback)
    {
        // Disable interrupts
        outportb(0x70, inportb(0x70) | 0x80);
        asm("cli");
        
        serial_rx_callback = callback;
        
        if (port == 0 || port == 2)
        {
            // Enable interrupt for IRQ4
            set_isr(serial0_isr, PIC1_IRQ(4));
            outportb(PIC1 + 1,(inportb(PIC1 + 1) & 0xEF));
        }
        else if (port == 1 || port == 3)
        {
            // Enable interrupt for IRQ3
            set_isr(serial1_isr, PIC1_IRQ(3));
            outportb(PIC1 + 1,(inportb(PIC1 + 1) & 0xF7));
        }
        
        unsigned int port_addr = port_address(port);
        outportb(port_addr + 1, 0x01);                          // Enable port interrupts for data available
        
        // Enable interrupts
        outportb(0x70, inportb(0x70) & 0x7F);
        asm("sti");
        
        // Enable PIC1 for next interrupt
        outportb(PIC1, 0x20);
    }
    else
    {
        unsigned int port_addr = port_address(port);
        outportb(port_addr + 1, 0x00);                          // Disable port interrupts
    }
}

byte serial_is_data_ready(PORT port)
{
    return inportb(port_address(port) + 5) & 1;
}

byte serial_is_transmit_empty(PORT port)
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
