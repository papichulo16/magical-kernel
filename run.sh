#!/bin/bash

#qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso -d guest_errors -s
#qemu-system-x86_64 -machine q35 -cdrom dist/x86_64/kernel.iso -d guest_errors -s    

qemu-system-x86_64 \
    -machine q35 \
    -cdrom dist/x86_64/kernel.iso \
    -drive file=disk.img,format=raw,if=none,id=ahci0 \
    -device ahci,id=ahci \
    -device ide-hd,bus=ahci.0,drive=ahci0 \
    -d guest_errors \
    -s
