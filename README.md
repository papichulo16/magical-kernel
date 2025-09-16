# Magical Kernel 
## The most magical kernel of all

### features

right now the working features are as follows:
 - 64 bit mode booting
 - interrupt handling
 - printing
 - keyboard input

### goal

the goal is to have working IO, separate kernel/user space, and a file system.......
I am pretty busy and lazy so this will be a VERY slow process, but we will see how it goes.

### building

build docker container: `docker build build -t whatever`

run docker container: `docker run --rm -it -v $(pwd):/root/env whatever /bin/bash`

inside docker container run `make build-x86_64`
outside container run `./run.sh`

