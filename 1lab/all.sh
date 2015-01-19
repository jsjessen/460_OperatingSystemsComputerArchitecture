#!/bin/sh

echo Compiling...
 # assemble 16-bit (object) code
 # compile and create a.out
if as86 -o bs.o bs.s && bcc -o main.o -c -ansi main.c; then
    echo OK
else
    exit 1
fi

echo Linking...
# A linker that combines the object code to generate a ONE-segment
# binary executable image. We shall use BCC under Linux to do (1) and (2).
if ld86 -d bs.o main.o util.o /usr/lib/bcc/libc.a; then # linker, don't need header so chop it off
    echo OK
else
    exit 1
fi

echo Checking size...  
size=$(stat -c %s a.out)
if [size -lt 1024]; then
then
    echo OK 
else
    exit 1
fi

echo Dumping... # a.out to first block of virtual floppy disk 
if dd if=a.out of=mtximage bs=1024 count=1 conv=notrunc; then
    echo OK
else
    exit 1

# don't truncate so only inserting to beginning, not deleting everything after
# The last step dumps (at most 1KB of) a.out to BLOCK 0 of FD.

#      The resulting disk should be bootable.

echo Running...  # virtual floppy disk on virtual i386 QEMU system 
if qemu-system-i386 -fda mtximage o-fd-bootchk -localtime -serial mon:stdio; then
    echo OK
else
    exit 1

#                         Contents of the MTX disk image:
#
#     |  B0  | B1 ...................................................... B1339 |
#     --------------------------------------------------------------------------
#     |booter|   An EXT2 file system for MTX; kernel=/boot/mtx                 | 
#     --------------------------------------------------------------------------
