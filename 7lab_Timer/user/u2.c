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
    printf("ENTERING U2");

    while(true)
    {
        pid = getpid();
        color = 0x000A + (pid % 6); 

        printf("\n----------------------------------------------------\n");
        printf("I AM P%d IN USER MODE ON SEGMENT %x WITH COUNT=%d\n", 
                getpid(), getcs(), count++);

        printf("COMMAND ? ");
        gets(name); 
        printf("\n");

        if(strlen(name) <= 0)
            continue;

        cmd_fptr = find_cmd(name);
        result = cmd_fptr();
    }
}
