.global     serial0_isr, serial1_isr
.align      4

serial0_isr:
    pushal
    cld
    calll serial0_rx_isr
    popal
    iretl

serial1_isr:
    pushal
    cld
    calll serial1_rx_isr
    popal
    iretl
