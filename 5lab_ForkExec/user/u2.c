#include "user.h"

int color;
int main()
{ 
    char name[64]; int pid, cmd;
    int count = 0;

    color = 0x000A;
    printf("\n==============================================\n");
    printf("Congratulations! You have made it to U2 :)");
    printf("\n==============================================\n");

    while(1)
    {
        pid = getpid();

        color = 0x000A + (pid % 6); 
        //color = 0x0C;

        printf("\n----------------------------------------------\n");
        printf("U2 COUNT = %d\n", count++);
        printf("I AM P%d IN U MODE: RUNNING SEGMENT=%x\n",getpid(), getcs()); 
        show_menu();
        printf("COMMAND ? ");
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

