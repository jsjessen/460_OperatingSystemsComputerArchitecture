#!/bin/sh

qemu-system-i386 -hda vdisk -smp 8 -m 512m -serial mon:stdio
