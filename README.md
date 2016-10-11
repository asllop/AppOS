# AppOS
*"Don't search an operating system for your application, make your App the OS"*

#### Cross Compiler

To compile your AppOS, first generate a GCC cross compiler, following the instructions in [OSDev tutorial](http://wiki.osdev.org/GCC_Cross-Compiler).

#### Prepare App

Write your app in main.c or add your own files and modify the Makefile.

#### Make

Just it, from a terminal run:

	$ make

Resulting binary is stored in **build** directory.

#### Run

The easiest way is using QEMU:

	$ qemu-system-i386 -kernel build/appos.elf
