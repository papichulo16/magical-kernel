#!/bin/bash

#qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso -d guest_errors -s
qemu-system-x86_64 -machine q35 -cdrom dist/x86_64/kernel.iso -d guest_errors -s   

