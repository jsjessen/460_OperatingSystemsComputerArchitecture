#!/bin/sh
. ~/Documents/460_OperatingSystemsComputerArchitecture/fancy_output.sh 

build_u1()
{
    dir=$1

    echo -n ${Cyan}
    echo ========================================== 
    echo U1 
    echo ------------------------------------------
    echo -n ${NC}

    try "Compiling booter C code..." \
        "bcc -o $dir/obj/u1.o -c -ansi $dir/u1.c"

    try "Compiling booter assembly code..." \
        "as86 -o $dir/obj/u.o $dir/u.s"

    try "Linking booter object code..." \
        "ld86 -o $dir/u1 $dir/obj/u.o $dir/obj/u1.o lib/mylib.a /usr/lib/bcc/libc.a"
}
