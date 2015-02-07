#include "string.h"

int atoi(char* str)
{
    int i;
    int value = 0;
    int len = strlen(str);

    for(i = len; i > 0; i--) 
        value += (str[len - i] - '0') * (int)pow(10, i - 1);

    return value;
}

int strlen(char* str)
{
    int len = 0;

    while(*(str++) != '\0')
        len++;

    return len;
}

int strcmp(char* str1, char* str2)
{
    while(*str1 != '\0' && *str2 != '\0' && *(str1++) == *(str2++));
        
    if(*str1 < *str2)
        return -1;
    if(*str1 == *str2)
        return 0;
    if(*str1 > *str2)
        return 1;
}

int strncmp(char* str1, char* str2, int n)
{
    while(n-- > 0 && *str1 != '\0' && *str2 != '\0' && *(str1++) == *(str2++));
        
    if(*str1 < *str2)
        return -1;
    if(*str1 == *str2)
        return 0;
    if(*str1 > *str2)
        return 1;
}

char* strcpy(char* dest, char* source)
{
    while(*source != '\0')
        *(dest++) = *(source++);

    *dest = '\0';
    return dest;
}

char* strncpy(char* dest, char* source, int n)
{
    while(n-- > 0 && *source != '\0')
        *(dest++) = *(source++);

    *dest = '\0';
    return dest;
}

int isspace(int c)
{
    if(c <= ' ')
        return 1;

    return 0;
}

int islower(int c)
{
    if('a' <= c && c <= 'z')
        return 1;
    
    return 0;
}

int isupper(int c)
{
    if('A' <= c && c <= 'Z')
        return 1;

    return 0;
}

int isalpha(int c)
{
    if(islower(c) || isupper(c))
        return 1;

    return 0;
}

int isdigit(int c)
{
    if('0' <= c && c <= '9')
        return 1;

    return 0;
}

int isalnum(int c)
{
    if(isalpha(c) || isdigit(c))
        return 1;

    return 0;
}
