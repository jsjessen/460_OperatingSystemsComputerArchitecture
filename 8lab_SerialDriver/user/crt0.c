#include "../lib/io.h"

int main(int argc, char* argv[]);

int color;
int main0(char* s)
{
    //tokenize s

    int argc = 0;
    char* argv[16];
    argv[0] = NULL;

    //color = 0x0C;
    //printf("\n        main0             \n");
    //printf("****************************\n");
    //printf("s = '%s'\n", s);
    //printf("****************************\n");

    // Having trouble compiling after adding parameters to u1.c's main()
    return main(argc, argv);
}
