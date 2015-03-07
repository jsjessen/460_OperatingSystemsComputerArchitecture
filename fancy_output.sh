#!/bin/sh

debug=0
#debug=1

Black='\033[0;30m'    
DarkGray='\033[1;30m'
Blue='\033[0;34m'    
LightBlue='\033[1;34m'
Green='\033[0;32m'   
LightGreen='\033[1;32m'
Cyan='\033[0;36m'     
LightCyan='\033[1;36m'
Red='\033[0;31m'     
LightRed='\033[1;31m'
Purple='\033[0;35m'     
LightPurple='\033[1;35m'
Orange='\033[0;33m'     
Gold='\033[1;33m'
Gray='\033[0;37m'     
White='\033[1;37m'
NC='\033[0m' # No Color

gold_echo()
{
    echo "${Gold}$1${NC}"
}

try()
{
    gold_echo "$1"

    shift
    for COMMAND in "$@"
    do
        if [ $debug -ne 0 ]; then
            echo -n ${LightRed}
            $COMMAND
            echo -n ${NC}
        else
            $COMMAND 2> /dev/null
        fi

        if [ $? -ne 0 ]; then
            #echo "${LightRed}FAILED${NC}"
            exit 1
        fi
    done

    #echo "${LightGreen}OK${NC}"
}
