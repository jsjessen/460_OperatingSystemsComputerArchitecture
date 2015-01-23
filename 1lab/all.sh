#!/bin/sh

vdisk="my_mtximage"

red='\033[0;31m'
green='\033[0;32m'
gold='\033[0;33m'
NC='\033[0m' # No Color

echo -n Replacing $vdisk with a fresh copy of the original... 
rm -f $vdisk
cp mtximage $vdisk
echo "${green}OK${NC}"

echo -n Compiling assembly code to 16-bit object code...
if as86 -o bs.o bs.s; 
then
    echo "${green}OK${NC}"
else
    exit 1
fi

echo -n Compiling C code to 16-bit object code...
if bcc -o main.o -c -ansi main.c; 
then
    echo "${green}OK${NC}"
else
    exit 1
fi

echo -n Linking object code into a one-segment binary executable image...
if ld86 -o booter -d bs.o main.o /usr/lib/bcc/libc.a 2> /dev/null; 
then # don't need header so chop it off
    echo "${green}OK${NC}"
else
    exit 1
fi

echo -n Verifying booter is within 1024 byte limit... 
size=$(stat -c %s booter)
if test $size -le 1024; 
then
    echo ${green}OK
    echo --------------------------------
    echo Booter is slim by `expr 1024 - $size` bytes 
    echo --------------------------------${NC}
else
    echo ${red}FAIL
    echo --------------------------------
    echo Reduce booter size by `expr $size - 1024` bytes 
    echo --------------------------------${NC}
    exit 1
fi

echo -n Dumping booter to first block of the virtual floppy disk... 
if dd if=booter of=$vdisk bs=1024 count=1 conv=notrunc 2> /dev/null; 
then
    echo "${green}OK${NC}"
else
    exit 1
fi

echo "${gold}Attempting to boot $vdisk on virtual 32-bit QEMU system...${NC}"
qemu-system-i386 -fda $vdisk -no-fd-bootchk -localtime -serial mon:stdio

#                         Contents of the MTX Disk Image:
#
#     |  B0  | B1 ...................................................... B1439 |
#     --------------------------------------------------------------------------
#     |booter|   An EXT2 file system for MTX; kernel=/boot/mtx                 | 
#     --------------------------------------------------------------------------
