#!/bin/sh

vm="qemu-system-i386"
kc_image="mtximage"
my_image="my_mtximage"
booter="../1lab/booter"
lib="mtxlib"
os="my_mtx"

red='\033[1;31m'
dark_red='\033[0;31m'
yellow='\033[0;33m'
green='\033[1;32m'
cyan='\033[0;36m'     
NC='\033[0m' # No Color

print()
{
    echo -n "${yellow}$*...${NC}"
}

try()
{
    for COMMAND in "$@"
    do
        echo "${cyan}"
        $COMMAND
        if [ $? -ne 0 ]; then
            echo "${red}FAILED${NC}"
            exit 1
        fi
    done

    echo "${green}OK${NC}"
}

# Must be super user to add os to image (mount)
try su 

print "Replacing $my_image with a fresh copy of ${kc_image}"
try "rm -f $my_image" \
    "cp $kc_image $my_image"

print "Compiling assembly code into 16-bit object code"
try "as86 -o ts.o ts.s"

print "Compiling C code into 16-bit object code"
try "bcc -o ${os}.o -c -ansi t.c"

print "Linking object code into a one-segment binary executable image"
try "ld86 -o $os -d ts.o ${os}.o $lib /usr/lib/bcc/libc.a" #2> /dev/null; 

print "Dumping $booter to first block of ${my_image}"
try "dd if=$booter of=$my_image bs=1024 count=1 conv=notrunc 2> /dev/null" 

print "Copying $os to $my_image/boot/${os}"
try "mount -o loop $my_image /mnt" \
    "cp $os /mnt/boot/$os" \
    "umount /mnt"

print "Attempting to boot $os from $my_image using ${vm}${NC}"
try "$vm -fda $my_image -no-fd-bootchk" #-localtime -serial mon:stdio
