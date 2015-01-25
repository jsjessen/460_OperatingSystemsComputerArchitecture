/******** myprint.h ********/

/* James Jessen
 * 10918967
 *
 * CptS 360
 * Lab #2
 */

#ifndef __MY_PRINT_H__
#define __MY_PRINT_H__

#define OCT 8
#define DEC 10
#define HEX 16

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

void rpu(u16 n, u16 base);
void prints(char *s);            // String
void printd(int n);              // Signed Int
void printl(u32 n);              // Unsigned Long
void printo(u16 n);              // OCTAL
void printx(u16 n);              // HEX
void myprintf(char *fmt, ...);   // Formatted

#endif // __MY_PRINT_H__
