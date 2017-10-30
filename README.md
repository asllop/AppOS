# AppOS
*"Don't use an operating system for your application, make your App the OS"*

#### 1. Cross Compiler

To compile your AppOS, first generate a GCC cross compiler, following the instructions in [OSDev tutorial](http://wiki.osdev.org/GCC_Cross-Compiler).

#### 2. Prepare App

Write your app in **app/main.c** or add your own files and modify the Makefile.

#### 3. Make

Just it, from a terminal run:

	$ make

Resulting binary is stored in **build** directory.

#### 4. Run

The easiest way is using QEMU:

	$ qemu-system-i386 -kernel build/appos.elf
