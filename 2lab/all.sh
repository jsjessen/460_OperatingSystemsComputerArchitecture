#!/bin/sh

vm="qemu-system-i386"
kc_image="mtximage"
my_image="my_mtximage"
booter="booter"
lib="mtxlib"
os="my_mtx"

red='\033[1;31m'
dark_red='\033[0;31m'
gold='\033[0;33m'
green='\033[1;32m'
cyan='\033[0;36m'     
NC='\033[0m' # No Color

gold_echo()
{
    echo -n "${gold}$*${NC}"
}

try()
{
    for COMMAND in "$@"
    do
        echo -n "${cyan}"
        $COMMAND
        if [ $? -ne 0 ]; then
            echo "${red}FAILED${NC}"
            exit 1
        fi
    done

    echo "${green}OK${NC}"
}

gold_echo "Replacing $my_image with a fresh copy of ${kc_image}..."
try "rm -f $my_image" \
    "cp $kc_image $my_image"

gold_echo "Compiling assembly code into 16-bit object code..."
try "as86 -o ts.o ts.s"

gold_echo "Compiling C code into 16-bit object code..."
try "bcc -o ${os}.o -c -ansi t.c"

gold_echo "Linking object code into a one-segment binary executable image..."
try "ld86 -o $os -d ts.o ${os}.o $lib /usr/lib/bcc/libc.a" #2> /dev/null; 
#try "ld86 -o $os -d ts.o ${os}.o /usr/lib/bcc/libc.a" #2> /dev/null; 

gold_echo "Dumping $booter to first block of ${my_image}..."
echo "dd if=$booter of=$my_image bs=1024 count=1 conv=notrunc" # 2> /dev/null" 
try "dd if=$booter of=$my_image bs=1024 count=1 conv=notrunc" # 2> /dev/null" 

gold_echo "Copying $os to $my_image/boot/${os}..."
try "sudo mount -o loop $my_image /mnt" \
    "sudo cp $os /mnt/boot/$os" \
    "sudo umount /mnt"

gold_echo "Attempting to boot $os from $my_image using ${vm}${NC}..."
echo "$vm -fda $my_image -no-fd-bootchk" #-localtime -serial mon:stdio
try "$vm -fda $my_image -no-fd-bootchk" #-localtime -serial mon:stdio
