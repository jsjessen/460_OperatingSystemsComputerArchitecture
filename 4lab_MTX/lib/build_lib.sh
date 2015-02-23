#!/bin/sh
. ~/Documents/460_OperatingSystemsComputerArchitecture/fancy_output.sh 

build_lib()
{
    dir=$1
    lib=$2

    echo -n ${Cyan}
    echo ========================================== 
    echo LIBRARY 
    echo ------------------------------------------
    echo -n ${NC}

    try "Compiling getc..." \
        "as86 -o $dir/obj/getc.o $dir/getc.s" 

    try "Compiling putc..." \
        "as86 -o $dir/obj/putc.o $dir/putc.s" 

    try "Compiling math..." \
        "bcc -o $dir/obj/math.o -c -ansi $dir/math.c" 

    try "Compiling string..." \
        "bcc -o $dir/obj/string.o -c -ansi $dir/string.c" 

    try "Compiling io..." \
        "bcc -o $dir/obj/io.o -c -ansi $dir/io.c" 

    try "Compiling queue..." \
        "bcc -o $dir/obj/queue.o -c -ansi $dir/queue.c"

    try "Compiling list..." \
        "bcc -o $dir/obj/list.o -c -ansi $dir/list.c" 

    try "Building library $dir/${lib}..." \
        "rm -f $dir/$lib" \
        "ar cr $dir/$lib  $dir/obj/io.o $dir/obj/getc.o $dir/obj/putc.o $dir/obj/queue.o $dir/obj/list.o"
}
