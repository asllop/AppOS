.global   bad_thing_isr, empty_int_isr
.align   4

bad_thing_isr:
    pushal
    cld
    calll bad_thing
    popal
    iretl

empty_int_isr:
    pushal
    cld
    calll empty_int
    popal
    iretl
