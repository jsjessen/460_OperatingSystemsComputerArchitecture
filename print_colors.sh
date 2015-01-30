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
Yellow='\033[1;33m'
Gray='\033[0;37m'     
White='\033[1;37m'

try()
{
    for COMMAND in "$@"
    do
        echo "${dark_red}"
        $COMMAND
        if [ $? -ne 0 ]; then
            echo "${red}FAILED${NC}"
            exit 1
        fi
    done

    echo "${green}OK${NC}"
}

echo "${Black}        Testing"
echo "${DarkGray}     Testing"
echo "${Blue}         Testing"
echo "${LightBlue}    Testing"
echo "${Green}        Testing"
echo "${LightGreen}   Testing"
echo "${Cyan}         Testing"
echo "${LightCyan}    Testing"
echo "${Red}          Testing"
echo "${LightRed}     Testing"
echo "${Purple}       Testing"
echo "${LightPurple}  Testing"
echo "${Orange}       Testing"
echo "${Yellow}       Testing"
echo "${Gray}         Testing"
echo "${White}        Testing"
