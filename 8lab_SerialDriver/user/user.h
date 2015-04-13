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
#include "../lib/string.h"

#define LEN 64

#define SYSCALL_GET_PID    0
#define SYSCALL_PS         1
#define SYSCALL_CHNAME     2
#define SYSCALL_KMODE      3
#define SYSCALL_TSWITCH    4
#define SYSCALL_WAIT       5
#define SYSCALL_FORK       6
#define SYSCALL_EXEC       7

#define SYSCALL_PIPE       30
#define SYSCALL_READ_PIPE  31
#define SYSCALL_WRITE_PIPE 32
#define SYSCALL_CLOSE_PIPE 33
#define SYSCALL_PFD        34

#define SYSCALL_GETC       90
#define SYSCALL_PUTC       91

#define SYSCALL_EXIT       99

extern int color;
extern char *cmd[];
extern int pd[2]; // Might need to go in Ux.c

// u.s
int getcs();
int syscall(int num, ...);

int show_menu();
int find_cmd(char *name);

// syscalls
int getpid();
int ps();
int chname();
int kmode();
int kswitch();
int wait();
int geti_(); // lib or this?
int exit();
int _getc();
int _putc(char c);
int fork();
int exec();

// Pipe syscalls
int pipe();
int read();
int write();
int close();
int pfd();
int test_pipe();

int invalid(char *name);

#endif
