#!/bin/sh
. ~/Documents/460_OperatingSystemsComputerArchitecture/fancy_output.sh 

vm="qemu-system-i386"
kc_image="mtximage"
my_image="my_mtximage"
booter="boot/booter"
lib="mylib.a"
os="my_mtx"

try "Rebuilding library: ${lib}..." \
    "rm -f $lib" \
    "bcc -o io.o -c -ansi io.c" \
    "bcc -o queue.o -c -ansi queue.c" \
    "bcc -o list.o -c -ansi list.c" \
    "ar cr $lib io.o queue.o list.o"

try "Replacing $my_image with a fresh copy of ${kc_image}..." \
    "rm -f $my_image" \
    "cp $kc_image $my_image"

try "Compiling assembly code into 16-bit object code..." \
    "as86 -o ts.o ts.s"

try "Compiling C code into 16-bit object code..." \
    "bcc -o ${os}.o -c -ansi main.c"

try "Linking object code into a one-segment binary executable image..." \
    "ld86 -o $os -d ts.o ${os}.o $lib /usr/lib/bcc/libc.a"

try "Dumping $booter to first block of ${my_image}..." \
    "dd if=$booter of=$my_image bs=1024 count=1 conv=notrunc"

try "Copying $os to $my_image/boot/${os}..." \
    "sudo mount -o loop $my_image /mnt" \
    "sudo cp $os /mnt/boot/$os" \
    "sudo umount /mnt"

gold_echo "Attempting to boot $os from $my_image using ${vm}..."
echo
$vm -fda $my_image -no-fd-bootchk #-localtime -serial mon:stdio
