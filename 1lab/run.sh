#!/bin/sh

# Use BCC to generate a one-segment binary executable a.out WITHOUT header
as86 -o bs.o  bs.s
bcc -c -ansi  t.c
ld86 -d bs.o t.o /usr/lib/bcc/libc.a

# dump a.out to a VIRTUAL FD disk:
dd if=a.out of=myvdisk bs=1024 count=1 conv=notrunc

# Boot up QEMU from the virtual FD disk:
qemu-system-i386 -fda myvdisk -no-fd-bootchk
