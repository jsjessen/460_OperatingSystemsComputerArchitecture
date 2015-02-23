#!/bin/sh
. ~/Documents/460_OperatingSystemsComputerArchitecture/fancy_output.sh 

build_booter()
{
    dir=$1
    booter=$2

    echo -n ${Cyan}
    echo ========================================== 
    echo BOOTER
    echo ------------------------------------------
    echo -n ${NC}

    try "Compiling booter assembly code..." \
        "as86 -o $dir/obj/bs.o $dir/bs.s"

    try "Compiling booter C code..." \
        "bcc -o $dir/obj/main.o -c -ansi $dir/main.c"

    try "Linking booter object code..." \
        "ld86 -o $dir/$booter -d $dir/obj/bs.o $dir/obj/main.o /usr/lib/bcc/libc.a"

    gold_echo "Verifying booter is within size limit..." \ 
    size=$(stat -c %s $dir/$booter)
    if test $size -le 1024; 
    then
        echo -n ${LightGreen}
        echo --------------------------------
        echo Booter is slim by `expr 1024 - $size` bytes 
        echo --------------------------------${NC}
    else
        echo -n ${LightRed}
        echo --------------------------------
        echo Reduce booter size by `expr $size - 1024` bytes 
        echo --------------------------------${NC}
        exit 1
    fi
}
