#!/bin/sh
. ~/Documents/460_OperatingSystemsComputerArchitecture/fancy_output.sh 


build_lib()
{
    dir=$1
    lib=$2

    try "Building library $dir/${lib}..." \
        "rm -f $dir/$lib" \
        "bcc -o $dir/io.o -c -ansi $dir/io.c" \
        "bcc -o $dir/queue.o -c -ansi $dir/queue.c" \
        "bcc -o $dir/list.o -c -ansi $dir/list.c" \
        "ar cr $dir/$lib $dir/io.o $dir/queue.o $dir/list.o"
}
