#!/bin/sh

function error_exit
{
    echo "$1" 1>&2
    exit 1
}

echo -n Compiling...
 # assemble 16-bit (object) code
 # compile and create a.out
if as86 -o bs.o bs.s && bcc -c -ansi main.c; then
    echo OK
else
    error_exit "FAIL"
fi

echo -n Linking...
# A linker that combines the object code to generate a ONE-segment
# binary executable image. We shall use BCC under Linux to do (1) and (2).
if ld86 -d bs.o main.o /usr/lib/bcc/libc.a; then # linker, don't need header so chop it off
    echo OK
else
    error_exit "FAIL"
fi

echo -n Checking size...  
size=$(stat -c %s a.out)
if [size -lt 1024]; then
then
    echo OK 
else
    error_exit "FAIL"
fi

echo -n Dumping... # a.out to first block of virtual floppy disk 
if dd if=a.out of=mtximage bs=1024 count=1 conv=notrunc; then
    echo OK
else
    error_exit "FAIL"
# don't truncate so only inserting to beginning, not deleting everything after
# The last step dumps (at most 1KB of) a.out to BLOCK 0 of FD.

#      The resulting disk should be bootable.

echo -n Running...  # virtual floppy disk on virtual i386 QEMU system 
if qemu-system-i386 -fda mtximage -no-fd-bootchk -localtime -serial mon:stdio; then
    echo OK
else
    error_exit "FAIL"

#                         Contents of the MTX disk image:
#
#     |  B0  | B1 ...................................................... B1339 |
#     --------------------------------------------------------------------------
#     |booter|   An EXT2 file system for MTX; kernel=/boot/mtx                 | 
#     --------------------------------------------------------------------------
