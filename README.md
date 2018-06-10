# AppOS
*"Make your App the OS"*

AppOS is a kernel designed to be embedded into an application, as if it were a static library. It features a small footprint memory management system, preemptive & cooperative multitasking with threads, networking, events, driver abstraction layer and more.

The main fields of aplication are:

- Internet of Things (IoT)
- Wearable
- Microcontrollers / Electronics projects
- Embedded systems in general
- Standalone applications
- Industrial automation
- Core for bigger Kernels / Operating Systems
- Learning about OS development

Designed to be small but scalable and modular, it could fit on a 8-bit microcontroller as well as on a full featured computer. It's based on a minimal super lightweight core, around which is possible to add multiple modules independently, making it highly customizable.

AppOS is multiplatform and designed to be easily ported. Currently supported platforms are:

- PC compatibles
- Raspberry PI (work in progress)
- Arduino UNO and compatibles (work in progress)
- BBC micro:bit (planned)
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

Depending on the app configuration and resources usage, you could need to add some arguments.

To redirect a serial port to stdio:

	$ qemu-system-i386 -serial stdio -kernel build/appos.elf

Or redirect a serial port to a virtual device serial port (in Linux):

	$ qemu-system-i386 -serial pty -kernel build/appos.elf
