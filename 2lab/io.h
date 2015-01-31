// James Jessen
// 10918967

#ifndef __MY_PRINT_H__
#define __MY_PRINT_H__


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

// Output
void rpu(u16 n, u16 base);
void prints(char *s);            // String
void printd(int n);              // Signed Int
void printu(u16 n);              // Unsigned Short 
void printl(u32 n);              // Unsigned Long
void printo(u16 n);              // OCTAL
void printx(u16 n);              // HEX
void myprintf(char *fmt, ...);   // Formatted


#endif // __MY_PRINT_H__
