.global     serial_isr
.align      4

serial_isr:
    pushal
    cld
    calll serial_rx_isr
    popal
    iretl
