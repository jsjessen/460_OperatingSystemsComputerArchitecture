#include <stdio.h>
#include <string.h>

int prints(char *s)
{
    int i = 0;
    while(*(s + i) != '\0')
    {
        putchar(*(s + i));
        i++;
    }
    return i;
}

int get_string(char s[])
{
    int i = 0;
    while(1)
    {
        char c = getchar();

        if(c == '\n' || c == '\r' || c == '\0')
            break;
        else
            s[i++] = c;
    }
    s[i] = '\0';
    return i;
}

int main(void)
{
    char name[64];
    while(1)
    {
        prints("What's your name? ");
        int glen = get_string(name);
        printf("get length = %d\n", glen);

        if (name[0]==0)
            break;

        prints("Welcome "); 
        putchar('|');
        int plen = prints(name); 
        putchar('|');
        printf("\nput length = %d\n", plen);
        prints("\n\r");
    }
    prints("return to assembly and hang\n\r");

    return 0;
}
