#!/usr/bin/env bash
set -e

IMAGE="disk.img"
SIZE_MB=100

# Create a 100MB empty disk image
dd if=/dev/zero of="$IMAGE" bs=1M count="$SIZE_MB" status=progress

# Create an ext2 filesystem inside the image
mkfs.ext2 -F "$IMAGE"
