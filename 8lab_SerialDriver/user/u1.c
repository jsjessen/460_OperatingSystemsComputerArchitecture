#include "user.h"

int main()//int argc, char* argv[])
{ 
    char name[64]; 
    int pid;
    int cmd;
    int count = 0;

    pid = getpid();
    color = 0x000A + (pid % 6); 
    printf("\n====================================================\n");
    printf("Entering U1");

    while(true)
    {
        pid = getpid();
        color = 0x000A + (pid % 6); 

        printf("\n----------------------------------------------------\n");
        printf("I am P%d in User Mode on Segment %x with count=%d\n", 
                getpid(), getcs(), count++);
        show_menu();

        printf("Command ? ");
        gets(name); 
        printf("\n");
        if(name[0] == 0) 
            continue;

        cmd = find_cmd(name);
        switch(cmd)
        {
            case  0 : getpid();     break;
            case  1 : ps();         break;
            case  2 : chname();     break;
            case  3 : kmode();      break;
            case  4 : kswitch();    break;
            case  5 : wait();       break;
            case  6 : exit();       break;
            case  7 : fork();       break;
            case  8 : exec();       break;

            case  9 : pipe();       break;
            case 10 : pfd();        break;
            case 11 : close();      break;
            case 12 : read();       break;
            case 13 : write();      break;
            case 14 : test_pipe();  break;

            case 15 : sin();        break;
            case 16 : sout();       break;

            default: invalid(name); break;
        }
    }
}
