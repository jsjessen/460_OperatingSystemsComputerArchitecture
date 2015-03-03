// James Jessen
// 10918967
// CptS 460

// ------------------------------------------ 
// MTX User Mode
// ------------------------------------------ 

#ifndef __KERNEL_H__
#define __KERNEL_H__


#include "../type.h"
#include "../lib/io.h"

#define LEN 64

extern char *cmd[];

// u.s
int getcs();
int syscall(int num, ...);

int show_menu();
int find_cmd(char *name);

// syscalls
int getpid();
int ps();
int chname();
int kfork();
int kswitch();
int wait();
int geti_(); // lib or this?
int exit();
int _exit(int exitValue);
int _getc();
int _putc(char c);
int fork();
int exec();

int invalid(char *name);

#endif
