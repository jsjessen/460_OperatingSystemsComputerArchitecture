#!/bin/sh
. ~/Documents/460_OperatingSystemsComputerArchitecture/fancy_output.sh 
. lib/build_lib.sh 
. boot/build_booter.sh
. user/build_u1.sh

boot_dir="boot"
booter="booter"

lib_dir="lib"
lib="mylib.a"

u1_dir="user"

os="my_mtx"

kc_image="disk/mtximage"
my_image="disk/my_mtximage"

vm="qemu-system-i386"

if [ "$1" = "all" ]; then
    build_booter $boot_dir $booter
    build_lib $lib_dir $lib
    build_u1 $u1_dir
fi

echo -n ${Cyan}
echo ========================================== 
echo OPERATING SYSTEM 
echo ------------------------------------------
echo -n ${NC}

try "Replacing $my_image with a fresh copy of ${kc_image}..." \
    "rm -f $my_image" \
    "cp $kc_image $my_image"

try "Compiling OS assembly code..." \
    "as86 -o obj/ts.o ts.s"

try "Compiling OS C code..." \
    "bcc -o obj/${os}.o -c -ansi main.c"

try "Linking OS object code..." \
    "ld86 -o $os -d obj/ts.o obj/${os}.o $lib_dir/$lib /usr/lib/bcc/libc.a"

try "Dumping $booter to first block of ${my_image}..." \
    "dd if=$boot_dir/$booter of=$my_image bs=1024 count=1 conv=notrunc"

try "Mounting ${my_image}..." \
    "sudo mount -o loop $my_image /mnt"

try "Copying $os to $my_image/boot/${os}..." \
    "sudo cp $os /mnt/boot/$os" 

try "Copying u1 to $my_image/bin/u1..." \
    "sudo cp user/u1 /mnt/bin/u1" 

try "Unmounting ${my_image}..." \
    "sudo umount /mnt"
    
gold_echo "Attempting to boot $os from $my_image using ${vm}..."
echo
$vm -fda $my_image -no-fd-bootchk #-localtime -serial mon:stdio
