#include "../lib/io.h"

int main(int argc, char* argv[]);

int color;
int main0(char* s)
{
    //tokenize s

    int argc = 0;
    char* argv[16];

    char arg1[32];
    char arg2[32];
    char arg3[32];

    strcpy(arg1, "one");
    strcpy(arg2, "two");
    strcpy(arg3, "three");

    argv[0] = arg1;
    argv[2] = arg2;
    argv[3] = arg3;
    argc = 3;

    color = 0x0C;
    printf("\n        main0             \n");
    printf("****************************\n");
    printf("s = '%s'\n", s);
    printf("****************************\n");

    return main(argc, argv);
}
