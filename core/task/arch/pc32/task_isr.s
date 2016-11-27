.global   pit_isr, fake_timer_isr
.align   4
 
pit_isr:
    pushal
    // C code following the sysV ABI requires DF to be clear on function entry
    cld
    // Send current ESP as an argument to function timer_interrupt
    pushl %esp
    calll timer_interrupt
    // Get the stack pointer for next task
    movl %eax, %esp
    popal
    iretl

fake_timer_isr:
    pushal
    // C code following the sysV ABI requires DF to be clear on function entry
    cld
    // Send current ESP as an argument to function timer_interrupt
    pushl %esp
    calll fake_timer_interrupt
    // Get the stack pointer for next task
    movl %eax, %esp
    popal
    iretl
