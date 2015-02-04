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

u16 pow(u16 base, u16 power)
{
    u16 i;
    u16 result;

    result = base;

    if(base < 0 || power < 0)
        return 0;

    if(power == 0)
        return 1;

    for(i = 1; i < power; i++)
        result *= base;

    return result;
}

u16 geti()
{
    char str[32];
    u16 result = 0;
    u16 len = 0;
    u16 i;

    gets(str);
    while(str[len])  
        len++; 

    for(i = len; i > 0; i--) 
        result += (str[len - i] - '0') * pow(DEC, i - 1);

    return result;
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
void printl(u16 n)
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
void printf(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt); // Initialize the argument list

    while(*fmt != '\0')
    {
        if(*fmt != '%')
        {
            // for each \n, spit out an extra \r
            if(putc(*fmt) == '\n')
                putc('\r'); 
        }
        else
        {
            fmt++; // bypass %
            switch(*fmt) // *fmt = char after %
            {
                case 'c': // char
                    putc(va_arg(args, int)); //compiler said use int
                    break;

                case 's': // string
                    prints(va_arg(args, char*));
                    break;

                case 'd': // int
                    printd(va_arg(args, int));
                    break;

                case 'u': // unsigned int
                    printu(va_arg(args, u32));
                    break;

                case 'l': // unsigned int
                    printl(va_arg(args, u32));
                    break;

                case 'o': // OCTAL 
                    printo(va_arg(args, u32));
                    break;

                case 'x': // HEX
                    printx(va_arg(args, u32));
                    break;

                case '%': // %% -> %
                    putc('%');
                    break;

                default: // unknown specifier
                    putc('%');
                    putc(*fmt);
                    break;

            }// switch(*fmt)
        }// if(%)

        fmt++;

    } // while(fmt)
} // myprintf()
