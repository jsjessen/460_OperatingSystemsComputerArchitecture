/************** t.c file **************************/
int prints(char *s)
{
    int i = 0;
    while(*(s + i) != '\0')
    {
        putc(*(s + i));
        i++;
    }
    return i;
}

int gets(char s[])
{
    int i = 0;
    while(1)
    {
        char c = getc();

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

int main()
{
    char name[64];
    while(1)
    {
        prints("What's your name? ");
        gets(name);

        if(name[0] == 0)
            break;

        prints("Welcome "); prints(name); prints("\n\r");
    }
    prints("return to assembly and hang\n\r");

    return 0;
}
