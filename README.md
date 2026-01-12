# Kat Kernel 
## The cat oriented kernel you wish existed sooner!!11!

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

the goal is to have working IO, separate kernel/user space, and a file system

I am pretty busy so this will be a VERY slow process, but we will see how it goes. I also have blog posts up documenting my learning progress [here](https://luisabraham.xyz/projects/1_kat_kernel.html)

### building

build docker container: `docker build build -t whatever`

run docker container: `docker run --rm -it -v $(pwd):/root/env whatever /bin/bash`

inside docker container run `make build-x86_64`
outside container run `./run.sh`

