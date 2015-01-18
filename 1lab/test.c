#include <stdio.h>
#include <string.h>

void chs(int bno)
{
    int linear_s = bno * 2;

    int num_c = 80;
    int num_h = 2;
    int num_s = 18;

    int c,h,s;

    c        = linear_s / (num_h * num_s);
    linear_s = linear_s % (num_h * num_s);

    h = linear_s / num_s;
    s = linear_s % num_s;

    printf("%d  [%d  %d  %d]\n", bno, c,h,s);
}

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
    int i = 0;

    printf("BLK  [C  H  S]\n");
    printf("--------------\n");

    for(i = 0; i < 1440; i++)
        chs(i);

  //  char name[64];
  //  while(1)
  //  {
  //      prints("What's your name? ");
  //      int glen = get_string(name);
  //      printf("get length = %d\n", glen);

  //      if (name[0]==0)
  //          break;

  //      prints("Welcome "); 
  //      putchar('|');
  //      int plen = prints(name); 
  //      putchar('|');
  //      printf("\nput length = %d\n", plen);
  //      prints("\n\r");
  //  }
  //  prints("return to assembly and hang\n\r");

    return 0;
}
