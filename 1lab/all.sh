#!/bin/sh

vdisk="mtximage"

echo -n Compiling assembly...
if as86 -o bs.o bs.s; then
    echo OK
else
    exit 1
fi

echo -n Compiling 16-bit object code...
if bcc -o main.o -c -ansi main.c; then
    echo OK
else
    exit 1
fi

echo -n Linking object code into a one-segment binary executable image...
if ld86 -d bs.o main.o util.o /usr/lib/bcc/libc.a; then # don't need header so chop it off
    echo OK
else
    exit 1
fi

echo -n Verifying a.out is less than 1024 bytes... 
size=$(stat -c %s a.out)
if [size -lt 1024]; then
then
    echo OK 
else
    exit 1
fi

echo -n Dumping a.out to first block of the virtual floppy disk... 
if dd if=a.out of=$vdisk bs=1024 count=1 conv=notrunc; then
    echo OK
else
    exit 1

echo -n Attempting to boot $vdisk on virtual 32-bit QEMU system...
if qemu-system-i386 -fda mtximage o-fd-bootchk -localtime -serial mon:stdio; then
    echo OK
else
    exit 1

#                         Contents of the MTX Disk Image:
#
#     |  B0  | B1 ...................................................... B1439 |
#     --------------------------------------------------------------------------
#     |booter|   An EXT2 file system for MTX; kernel=/boot/mtx                 | 
#     --------------------------------------------------------------------------
