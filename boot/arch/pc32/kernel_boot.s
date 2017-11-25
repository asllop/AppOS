# Declare constants for the multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bss
.align 16
stack_bottom:
# Amount of memory for initial kernel stack
.skip 512
stack_top:

.section .text
.global _start
.type _start, @function
_start:
	mov $stack_top, %esp
	mov %ebx, grub_struct
	push %eax
	mov $section_after, %eax
	mov %eax, kernel_end
 	pop %eax

	call kernel_init

hlt_loop:
	cli
    hlt
	jmp hlt_loop

.global grub_struct, kernel_end

grub_struct: 	.long 0
kernel_end: 	.long 0

# Set the size of the _start symbol to the current location '.' minus its start.
# This is useful when debugging or when you implement call tracing.
.size _start, . - _start

# It marks the end of the kernel, since in the linker we ensure this sections goes at the end
.align 16
.section .after
section_after:
.byte 'A','p','p','O','S',' ','O','K'
.byte 0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55

