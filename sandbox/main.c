#include "lib/io.h"
#include "lib/string.h"

int main()
{
    char sh[64] = "James";
    char sj[64];
    char a[16] = "test";
    char b[16] = "kfork";
    char s[16];
    char* sp;
    char x[6] = "32767";
    char y[6] = "32768";


    printf("---------------\n");
    strcpy(sj, sh);

    printf("\"%s\"\n", sj);

    printf("---------------\n");
    if(strcmp(b,a) == 0)
        printf("%s == %s\n", a, b);
    else
        printf("%s != %s\n", a, b);
    printf("---------------\n");


    printf("len a = %d\n", strlen(a));
    printf("len b = %d\n", strlen(b));
    printf("---------------\n");
    
    //sp = gets(s);
    //printf("s = \"%s\"\n", s);
    //printf("sp = \"%s\"\n", sp);
    printf("---------------\n");
    strncat(a, b, 2);
    printf("cat = \"%s\"\n", a);
    printf("---------------\n");
    printf("x = %d should be %s\n", atoi(x), x);
    printf("y = %d should be %s\n", atoi(y), y);
    //printf("z = %d should be %s\n", atoi(z), z);
    printf("---------------\n");

    return 0;
}
