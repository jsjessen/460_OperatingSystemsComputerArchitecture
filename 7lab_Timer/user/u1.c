#include "cmd.h"

int color;

int main()//int argc, char* argv[])
{ 
    char input[MAX_INPUT_SIZE]; 
    int (*cmd_fptr)();
    int result;
    int count = 0; // for demo/debug

    color = 0x000A + (getpid() % 6); 
    printf("\n====================================================\n");
    printf("Entering U1");

    while(true)
    {
        printf("\n----------------------------------------------------\n");

        while(input[0] == '\0' || input[0] == '\n' || strlen(input) > MAX_INPUT_SIZE)
        {
            input[0] = '\0';
            printf("Proc%d:Seg%x:User1(%d)$ ", getpid(), getcs(), count++);
            gets(input); 
            printf("\n");
        }

        cmd_fptr = find_cmd(input);
        result = cmd_fptr();
    }
}
