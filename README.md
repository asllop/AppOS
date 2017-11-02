# AppOS
*"Don't use an operating system for your application, make your App the OS"*

AppOS is a kernel designed to be embedded into an application, as if it were a static library.

Its main fields of aplication are:

- Internet of Things (IoT)
- Standalone applications
- Microcontrollers / Electronics projects
- Industrial automation
- Embedded systems in general
- Core for bigger Kernels / Operating Systems
- Learning about OS development

Designed to be small but scalable and modular, it could fit on a 8-bit microcontroller as well as on a full featured computer. It's based on a minimal super lightweight core, around which is possible to add multiple modules independently, making it highly customizable.

AppOS is multiplatform and designed to be easily ported. Currently supported platforms are:

- i386 compatible PC
- Raspberry PI (work in progress)
- Arduino (work in progress)
- PIC microcontrollers and dev boards (planned)
- MSP430 and LaunchPads (planned)
- Others on demand

------

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
