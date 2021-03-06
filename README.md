# AppOS
*"Make your App the OS"*

AppOS is a kernel designed to be embedded into an application, as if it were a static library. It features a small footprint memory management system, preemptive & cooperative multitasking with threads, networking, events, driver abstraction layer, Real-Time oriented design and more. All its API functions are thread safe, don't stop the normal task scheduling and many of them are fully reentrant.

The main fields of aplication are:

- Internet of Things (IoT)
- Wearables
- Firmware
- Microcontrollers / Electronics projects
- Embedded systems in general
- Standalone applications
- Industrial automation
- Core for bigger Kernels / Operating Systems
- Learning about OS development

Designed to be small but scalable and modular, it could fit in a 8-bit microcontroller as well as in a full featured computer. It's based on a super lightweight core, around which is possible to add multiple modules independently, making it highly customizable.

AppOS is multiplatform and designed to be easily ported. Currently supported platforms are:

- PC compatibles
- Raspberry PI (work in progress)
- Arduino UNO and compatibles (work in progress)
- BBC micro:bit (planned)
- PIC microcontrollers and dev boards (planned)
- MSP430 and LaunchPads (planned)
- Others on demand

AppOS versioning schema conforms to [Semantic Versioning](https://semver.org/) rules.

------

#### 1. Cross Compiler

To compile your AppOS, first generate a GCC cross compiler, following the instructions in [OSDev tutorial](http://wiki.osdev.org/GCC_Cross-Compiler).

#### 2. Prepare App

Write your app in **app/main.c** or add your own files and modify the Makefile.

#### 3. Make

From a terminal run:

	$ make

The resulting binary will be stored in **build** directory.

#### 4. Run

The easiest way is using QEMU:

	$ qemu-system-i386 -kernel build/appos.elf

Depending on the app configuration and resources used, you may need some additional arguments. If you are using the serial port as a terminal interface for printing logs and reading user input, you may want to redirect the serial port to stdio so you can interact with your appos:

	$ qemu-system-i386 -serial stdio -kernel build/appos.elf

You can also use the serial port as a SLIP interface to connect your appos to the internet. In this case you would like to redirect the serial port to a virtual device (Linux only):

	$ qemu-system-i386 -serial pty -kernel build/appos.elf
