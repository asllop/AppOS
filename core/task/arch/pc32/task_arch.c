#include <task/task_internal.h>
#include <task/task.h>
#include <sys/sys.h>
#include <sys/io/arch/pc32/io.h>
#include <mem/arch/pc32/mem_arch.h>

#define FAKE_TIMER_INT      0xA0
#define TIMER_FREQ_HZ       100
#define MILLIS_PER_TICK     (1000 / TIMER_FREQ_HZ)

extern void pit_isr();
extern void fake_timer_isr();

extern uint16_t mem_code_selector;

void *push32(void *stackPointer, uint32_t value)
{
    stackPointer -= 4;
    *((uint32_t *)stackPointer) = value;
    return stackPointer;
}

// Setup an empty context on the stack for a new task
void task_context_init(struct TaskStruct *slot)
{
    void *currentStackPointer = slot->stack;
    
    // The place to go when the task ends and function returns
    currentStackPointer = push32(currentStackPointer, (uint32_t)task_end_point);
    
    // This will be poped by IRET on pit_isr()
    
    // Push EFLAGS
    currentStackPointer = push32(currentStackPointer, 0b1000000010);            // Bit 1 set (reserved), Interrupt flag set
    // Push CS
    currentStackPointer = push32(currentStackPointer, mem_code_selector * 8);   // CS selector on GDT
    // Push EIP
    currentStackPointer = push32(currentStackPointer, (uint32_t)slot->task);
    
    // This will be poped by POPA on pit_isr()
    
    // Push EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI
    currentStackPointer = push32(currentStackPointer, 0);
    currentStackPointer = push32(currentStackPointer, 0);
    currentStackPointer = push32(currentStackPointer, 0);
    currentStackPointer = push32(currentStackPointer, 0);
    currentStackPointer = push32(currentStackPointer, (uint32_t)slot->stack);
    currentStackPointer = push32(currentStackPointer, 0);
    currentStackPointer = push32(currentStackPointer, 0);
    currentStackPointer = push32(currentStackPointer, 0);
    
    slot->stack = currentStackPointer;
}

void *timer_interrupt(void *stackPointer)
{
    // Disable NMI
    outportb(0x70, inportb(0x70) | 0x80);
    
    systemTimestamp += MILLIS_PER_TICK;
    
    void *newStackPointer = task_schedule(stackPointer);
    
    // Enable NMI
    outportb(0x70, inportb(0x70) & 0x7F);
    
    // End of PIC1 interrupt (enable for next interrupt)
    outportb(PIC1, 0x20);
    
    return newStackPointer;
}

void *fake_timer_interrupt(void *stackPointer)
{
    systemTimestamp -= MILLIS_PER_TICK;
    
    return timer_interrupt(stackPointer);
}

// Init PIT (Programable Interval Timer)
void init_pit(unsigned int freq)
{
    // Disable NMI (not necessary, because we will do it on every port select)
    outportb(0x70, inportb(0x70) | 0x80);
    asm("cli");
    
    // Set Timer ISR
    set_isr(pit_isr, PIC1_IRQ(0));
    set_isr(fake_timer_isr, FAKE_TIMER_INT);

    // Setup PIT configuration
    unsigned int counter = 1193182 / freq;  // Base clock freq == 1193181.6666 Hz, we round to 1193182
    outportb(0x43, 0b110110);               // Send command (binary counter, mode 2, read/write LSB-MSB, counter 0)
    outportb(0x40, 0xFF & counter);         // Send counter (LSB)
    outportb(0x40, 0xFF & (counter >> 8));  // Send counter (MSB)
    
    // Enable only IRQ0 (timer)
    outportb(PIC1 + 1, 0b11111110);
    
    // Enable NMI again
    outportb(0x70, inportb(0x70) & 0x7F);
    asm("sti");
}

void task_force_scheduling()
{
    // Call a fake timer interrupt to avoid incrementing timestamp
    asm(
        "int %0;"
        :
        : "i" (FAKE_TIMER_INT)
        );
}

void task_scheduler_init()
{
    systemTimestamp = 0;
    
    init_pit(TIMER_FREQ_HZ);
}

extern void main_task();

void task_setup_stack(void *stackPointer)
{
    // Use main task stack as current stack
    
    asm("cli");
    
    asm(
        "movl %0, %%eax\n"
        "movl %%eax, %%esp\n"
        : : "m" (stackPointer)
        );
    
    // Enable multitasking
    core_permit();
    asm("sti");
    
    // Jump to main task
    asm("jmp main_task");
    
    // We should never get here
    core_fatal("Reached end of setup_stack");
}
