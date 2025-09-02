# Magical Kernel Ultra
## The most magical kernel of all

### building

build docker container: `docker build build -t whatever`

run docker container: `docker run --rm -it -v $(pwd):/root/env whatever /bin/bash`

inside docker container run `make <whatever>`
outside container run `./run.sh`

