# Magical Kernel 
## The most magical kernel of all

### features

right now the working features are as follows:
 - interrupt handling
 - exception handling
 - multithreading in kernel space
 - multithread synchronization 
 - virtual memory
 - kernel slab allocator
 - input/output
 - terminal UI and command handling

### goal

the goal is to have working IO, separate kernel/user space, and a file system.......
I am pretty busy and lazy so this will be a VERY slow process, but we will see how it goes.

### building

build docker container: `docker build build -t whatever`

run docker container: `docker run --rm -it -v $(pwd):/root/env whatever /bin/bash`

inside docker container run `make build-x86_64`
outside container run `./run.sh`

### Resources

 - https://wiki.osdev.org/Interrupts
 - https://wiki.osdev.org/Interrupt_Descriptor_Table
 - https://wiki.osdev.org/Interrupt_Service_Routines
 - https://wiki.osdev.org/8259_PIC
 - https://wiki.osdev.org/I8042_PS/2_Controller
 - https://wiki.osdev.org/Interrupts_Tutorial
 - https://wiki.osdev.org/Global_Descriptor_Table
 - https://wiki.osdev.org/Inline_Assembly/Examples#I/O_access
 - https://wiki.osdev.org/Programmable_Interval_Timer
 
 - https://youtu.be/FkrpUaGThTQ


