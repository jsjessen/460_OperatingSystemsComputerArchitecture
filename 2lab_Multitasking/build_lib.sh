#!/bin/sh

lib="mylib.a"

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

gold_echo "Rebuilding ${lib}..."
try "rm -f $lib" \
    "bcc -o io.o -c -ansi io.c" \
    "bcc -o queue.o -c -ansi queue.c" \
    "bcc -o list.o -c -ansi list.c" \
    "ar cr $lib io.o queue.o list.o" \
    "ar t $lib"
