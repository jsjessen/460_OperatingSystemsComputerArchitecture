#include "user.h"

int color;
int main()//int argc, char* argv[])
{ 
    char name[64]; int pid, cmd;
    int count = 0;

    pid = getpid();
    color = 0x000A + (pid % 6); 
    printf("\n\n====================================================\n");
    printf("Entering U1");

    while(1)
    {
        pid = getpid();
        color = 0x000A + (pid % 6); 

        printf("\n----------------------------------------------------\n");
        printf("I am P%d in User Mode on segment=%x with count=%d\n", 
                getpid(), getcs(), count++);
        show_menu();
        printf("Command ? ");
        gets(name); 
        printf("\n");
        if (name[0]==0) 
            continue;

        cmd = find_cmd(name);
        switch(cmd){
            case 0 : getpid();   break;
            case 1 : ps();       break;
            case 2 : chname();   break;
            case 3 : kfork();    break;
            case 4 : kswitch();  break;
            case 5 : wait();     break;
            case 6 : exit();     break;
            case 7 : fork();     break;
            case 8 : exec();     break;

            default: invalid(name); break;
        }
    }
}
