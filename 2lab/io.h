// James Jessen
// 10918967

#ifndef __IO_H__
#define __IO_H__

#include <stdarg.h>

#define OCT 8
#define DEC 10
#define HEX 16

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

// Imported from ts.s 
char getc(void);
char putc(char c);
void* get_ebp(void);
void* get_esp(void);

// Input
char* gets(char str[]);
u16 geti(void);

// Output
void rpu(u16 n, u16 base);
void prints(char *s);         // String
void printd(int n);           // Signed Int
void printu(u16 n);           // Unsigned Short 
void printl(u16 n);           // Unsigned Long
void printo(u16 n);           // OCTAL
void printx(u16 n);           // HEX
void printf(char *fmt, ...);  // Formatted


#endif
