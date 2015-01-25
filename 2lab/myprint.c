// James Jessen
// 10918967

#include "myprint.h"

void rpu(u16 n, u16 base)
{
    static char *table = "0123456789ABCDEF";
    char c;

    if(n)
    {
        c = table[n % base];
        rpu(n / base, base);
        putc(c);
    }
} 

// String
void prints(char* s)
{
    char *cp = s;

    while(*cp)
        putc(*cp++);
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
    // 12 / 4 = 3
    int *ip = ebp + 3;

    while(*cp)
    {
        if (*cp != '%')
        {
            // for each \n, spit out an extra \r
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
