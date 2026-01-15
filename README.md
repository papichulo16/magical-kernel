# Magical-Kernel Ultra 
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
 - drivers:
   - AHCI SATA driver
   - PS/2 Keyboard

### goal

TO DO GOALS:
 - FAT or EXT2 file system driver
 - ELF driver
 - user space, syscall trap frame
 - dynamic linker??
 - buddy allocator??

I am pretty busy so this will be a VERY slow process, but we will see how it goes. I also have blog posts up documenting my learning progress [here](https://luisabraham.xyz/projects/1_kernel.html)

### building

build docker container: `docker build build -t whatever`

run docker container: `docker run --rm -it -v $(pwd):/root/env whatever /bin/bash`

inside docker container run `make build-x86_64`
outside container run `./run.sh`

