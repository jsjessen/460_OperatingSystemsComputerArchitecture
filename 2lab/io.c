// James Jessen
// 10918967

#include "io.h"


// Assumes memory for str has been allocated
char* gets(char str[])
{
    while((*(str) = getc()) != '\r')
        putc(*str++); // So user can see what they're typing

    *str = '\0'; // Append with null char
    return str;
}

void rpu(u16 n, u16 base)
{
    static char *table = "0123456789ABCDEF";

    if(n)
    {
        rpu(n / base, base);
        putc(table[n % base]);
    }
} 

// String
void prints(char* str)
{
    while(*str)
        putc(*str++);
}

// Signed Int
void printd(int n)
{
    if(n == 0)
        putc('0');
    else if(n > 0)
        rpu(n, DEC);
    else
    {
        putc('-');
        rpu(-n, DEC);
    }
}

// Unsigned Short 
void printu(u16 n)
{
    if(n == 0)
        putc('0');
    else
        rpu(n, DEC);
}

// Unsigned Long
void printl(u32 n)
{
    if(n == 0)
        putc('0');
    else
        rpu(n, DEC);
}

// Unsigned OCTAL
void printo(u16 n)
{
    putc('0');

    if (n == 0)
        putc('0');
    else
        rpu(n, OCT);
}

// Unsigned HEX
void printx(u16 n)
{
    putc('0');
    putc('x');

    if (n == 0)
        putc('0');
    else
        rpu(n, HEX);
}

// Formatted Printing
void myprintf(char *fmt, ...)
{
    //                12    8      4      0
    // ... d | c | b | a | fmt | retPC | ebp | locals ...

    int *ebp = (int*)get_ebp();
    char *cp = fmt;

    // each int ptr increment = 4 bytes 
    int *ip = ebp + (12 / 4);

    while(*cp)
    {
        if (*cp != '%')
        {
            // for each \n, also \r
            if (putc(*cp) == '\n')
                putc('\r'); 
        }
        else
        {
            cp++; // bypass %
            switch (*cp) // *cp = char after %
            {
                case 'c': // char
                    putc(*ip++);
                    break;

                case 's': // string
                    prints((char*)(*ip++));
                    break;

                case 'd': // signed int 
                    printd(*ip++);
                    break;

                case 'u': // unsigned short 
                    printu(*ip++);
                    break;

                case 'l': // unsigned long 
                    printl(*ip++);
                    break;

                case 'o': // OCTAL 
                    printo(*ip++);
                    break;

                case 'x': // HEX
                    printx(*ip++);
                    break;

                case '%': // %% -> %
                    putc('%');
                    break;

                default: // unknown specifier
                    putc('%');
                    putc(*cp);
                    break;
            }// switch(*cp)
        }// if(%)

        cp++;

    } // while(*cp)
} // myprintf()
