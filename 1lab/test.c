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

        if(c == '\0' || c == '\n')
        {
            s[i++] = '\0';
            break;
        }
        else
            s[i++] = c;
    }
    return i;
}

int main(void)
{
    char name[64];
    while(1)
    {
        prints("What's your name? ");
        get_string(name);

        if (name[0]==0)
            break;

        prints("Welcome "); prints(name); prints("\n\r");
    }
    prints("return to assembly and hang\n\r");

    return 0;
}
