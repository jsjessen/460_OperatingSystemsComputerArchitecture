#!/bin/sh
. ~/Documents/460_OperatingSystemsComputerArchitecture/fancy_output.sh 

build_lib()
{
    dir=$1
    lib=$2

    try "Compiling getc..." \
        "as86 -o $dir/getc.o $dir/getc.s" 

    try "Compiling putc..." \
        "as86 -o $dir/putc.o $dir/putc.s" 

    try "Compiling math..." \
        "bcc -o $dir/math.o -c -ansi $dir/math.c" 

    try "Compiling string..." \
        "bcc -o $dir/string.o -c -ansi $dir/string.c" 
    
    try "Compiling io..." \
        "bcc -o $dir/io.o -c -ansi $dir/io.c" 

    try "Compiling queue..." \
        "bcc -o $dir/queue.o -c -ansi $dir/queue.c"

    try "Compiling list..." \
        "bcc -o $dir/list.o -c -ansi $dir/list.c" 

    try "Building library $dir/${lib}..." \
        "rm -f $dir/$lib" \
        "ar cr $dir/$lib  $dir/io.o $dir/getc.o $dir/putc.o $dir/queue.o $dir/list.o"
}
