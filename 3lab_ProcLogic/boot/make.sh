#!/bin/sh
. ~/Documents/460_OperatingSystemsComputerArchitecture/fancy_output.sh 

vm="qemu-system-i386"
original="../mtximage"
mydisk="../my_mtximage"

try "Replacing $mydisk with a fresh copy of the original..." \
    "rm -f $mydisk" \
    "cp $original $mydisk"

try "Compiling assembly code to 16-bit object code..." \
    "as86 -o bs.o bs.s"

try "Compiling C code to 16-bit object code..." \
    "bcc -o main.o -c -ansi main.c"

try "Linking object code into a one-segment binary executable image..." \
    "ld86 -o booter -d bs.o main.o /usr/lib/bcc/libc.a"

gold_echo "Verifying booter is within 1024 byte limit..."
size=$(stat -c %s booter)
if test $size -le 1024; 
then
    echo ${LightGreen}OK
    echo --------------------------------
    echo Booter is slim by `expr 1024 - $size` bytes 
    echo --------------------------------${NC}
else
    echo ${LightRed}FAIL
    echo --------------------------------
    echo Reduce booter size by `expr $size - 1024` bytes 
    echo --------------------------------${NC}
    exit 1
fi

try "Dumping booter to first block of the virtual floppy disk..." \
    "dd if=booter of=$mydisk bs=1024 count=1 conv=notrunc"

if [ "$1" = "run" ]; then
    gold_echo "Attempting to boot $mydisk on virtual 32-bit QEMU system..."
    echo
    $vm -fda $mydisk -no-fd-bootchk -localtime -serial mon:stdio
fi

#                         Contents of the MTX Disk Image:
#
#     |  B0  | B1 ...................................................... B1439 |
#     --------------------------------------------------------------------------
#     |booter|   An EXT2 file system for MTX; kernel=/boot/mtx                 | 
#     --------------------------------------------------------------------------
