#!/bin/sh
. ~/Documents/460_OperatingSystemsComputerArchitecture/fancy_output.sh 
. lib/build_lib.sh 
. boot/build_booter.sh

boot_dir="boot"
booter="booter"

lib_dir="lib"
lib="mylib.a"

os="my_mtx"

kc_image="disk/mtximage"
my_image="disk/my_mtximage"

vm="qemu-system-i386"


if [ "$1" = "all" ]; then
    build_booter $boot_dir $booter
    echo "${Purple}===========================================================================${NC}"
    build_lib $lib_dir $lib
    echo "${Purple}===========================================================================${NC}"
fi

try "Replacing $my_image with a fresh copy of ${kc_image}..." \
    "rm -f $my_image" \
    "cp $kc_image $my_image"

try "Compiling OS assembly code..." \
    "as86 -o ts.o ts.s"

try "Compiling OS C code..." \
    "bcc -o ${os}.o -c -ansi main.c"

try "Linking OS object code..." \
    "ld86 -o $os -d ts.o ${os}.o $lib_dir/$lib /usr/lib/bcc/libc.a"

try "Dumping $booter to first block of ${my_image}..." \
    "dd if=$booter of=$my_image bs=1024 count=1 conv=notrunc"

try "Copying $os to $my_image/boot/${os}..." \
    "sudo mount -o loop $my_image /mnt" \
    "sudo cp $os /mnt/boot/$os" \
    "sudo umount /mnt"

gold_echo "Attempting to boot $os from $my_image using ${vm}..."
echo
$vm -fda $my_image -no-fd-bootchk #-localtime -serial mon:stdio
