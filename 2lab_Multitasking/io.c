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

//u16 count_spec(char* str)
//{
//    u16 count;
//
//    count = 0;
//    while(*str)
//    {
//        if(*str == '%')
//        {
//            str++;
//            switch(*str) // *str = char after %
//            {
//                case 'c': // char
//                case 's': // string
//                case 'd': // signed int 
//                case 'u': // unsigned short 
//                case 'l': // unsigned long 
//                case 'o': // OCTAL 
//                case 'x': // HEX
//                    count++;
//                default: // unknown specifier
//                    break;
//            }
//        }
//        str++;
//    } 
//    return count;
//}

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
//    u16* ebp;
//    char* cp;
//    u16* ip;
//    int i;
//
//    //             8   6    4      2      0
//    // ... a | b | c | d | fmt | retPC | ebp | locals ...
//    ebp = get_ebp();
//    ip = (ebp - (6 / 2)) - count_spec(fmt);
//    //putc('\n');
//    putc('*');
//    //for(i = 0; i < 8; i++)
//    //{
//    //    prints(" - ");
//    //    printu(*ip++);
//    //}
//    //ip--; // Fixes debug in Init
//    //ip++; // Fixes debug in Main, somewhere the stack frame is off by 2 bytes
//    // None fixes list/queue printing 
//
//    cp = fmt;
//    while(*cp)
//    {
//        if (*cp != '%')
//        {
//            // for each \n, also \r
//            if (putc(*cp) == '\n')
//                putc('\r'); 
//        }
//        else
//        {
//            cp++; // bypass %
//            switch (*cp) // *cp = char after %
//            {
//                case 'c': // char
//                    putc(*ip++);
//                    break;
//
//                case 's': // string
//                    prints((char*)(*ip++));
//                    break;
//
//                case 'd': // signed int 
//                    printd(*ip++);
//                    break;
//
//                case 'u': // unsigned short 
//                    printu(*ip++);
//                    break;
//
//                case 'l': // unsigned long 
//                    printl(*ip++);
//                    break;
//
//                case 'o': // OCTAL 
//                    printo(*ip++);
//                    break;
//
//                case 'x': // HEX
//                    printx(*ip++);
//                    break;
//
//                case '%': // %% -> %
//                    putc('%');
//                    break;
//
//                default: // unknown specifier
//                    putc('%');
//                    putc(*cp);
//                    break;
//            }// switch(*cp)
//        }// if(%)
//
//        cp++;
//
//    } // while(*cp)
//} // myprintf()
