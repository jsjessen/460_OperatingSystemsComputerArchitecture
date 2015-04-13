#include "cmd.h"

int main()//int argc, char* argv[])
{ 
    char name[64]; 
    int result;
    int (*cmd_fptr)();

    int pid;
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

        printf("Command ? ");
        gets(name); 
        printf("\n");

        if(strlen(name) <= 0)
            continue;

        cmd_fptr = find_cmd(name);
        result = cmd_fptr();
    }
}
