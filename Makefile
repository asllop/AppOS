CC=i686-elf-gcc
AS=i686-elf-as
#CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra -I./include -I./core -I./ -g
CFLAGS=-std=gnu99 -ffreestanding -O0 -Wall -Wextra -I./include -I./core -I./ -g
OUTPUT=build/appos.elf
OBJECTS= \
    boot/arch/pc32/kernel_boot.o \
	boot/cmdline_args.o \
	boot/start.o \
	core/mem/arch/pc32/mem_arch.o \
	core/mem/mem_internal.o \
	core/mem/mem.o \
	core/sys/sys.o \
	core/sys/sys_internal.o \
	core/sys/arch/pc32/sys_arch.o \
	core/sys/io/arch/pc32/io.o \
    core/sys/io/arch/pc32/io_isr.o \
	core/task/task.o \
	core/task/task_internal.o \
	core/task/arch/pc32/task_arch.o \
    core/task/arch/pc32/task_isr.o \
	event/event_internal.o \
	event/event.o \
    serial/arch/pc32/serial_arch.o \
    serial/serial.o \
    term/term_internal.o \
    term/term.o \
    term/drivers/term_serial/term_serial.o \
    term/ansi.o \
    net/net.o \
    net/slip/slip.o \
    net/slip/slip_internal.o \
    net/ipv4/ipv4.o \
    net/ipv4/ipv4_internal.o \
    net/udp/udp.o \
    net/net_utils/net_utils.o \
    lib/NQCLib/NQCLib.o \
    app/utils.o \
    app/main_testmem.o
#	app/main_net.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.s
	$(AS) $< -o $@

all: $(OBJECTS) link

link:
	$(CC) -T linker.ld -o $(OUTPUT) -ffreestanding -O2 -nostdlib $(OBJECTS) -lgcc

clean:
	rm -f $(OBJECTS)
	rm -f $(OUTPUT)
