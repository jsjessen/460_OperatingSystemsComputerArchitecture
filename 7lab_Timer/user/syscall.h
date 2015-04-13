// James Jessen
// 10918967
// CptS 460

// ------------------------------------------ 
// Syscalls to Kernel 
// ------------------------------------------ 

#ifndef __SYSCALL_H__
#define __SYSCALL_H__


#include "type.h"
#include "lib/io.h"

typedef enum
{
    GET_PID    = 0,
    PS         = 1,
    CHNAME     = 2,
    KMODE      = 3,
    TSWITCH    = 4,
    WAIT       = 5,
    FORK       = 6,
    EXEC       = 7,

    PIPE       = 30,
    READ_PIPE  = 31,
    WRITE_PIPE = 32,
    CLOSE_PIPE = 33,
    PFD        = 34,

    CHCOLOR    = 50,

    GETC       = 90,
    PUTC       = 91,

    EXIT       = 99
} syscall_t;

// u.s int getcs();
int syscall(syscall_t code, ...);

int getpid();
int getc();
int putc(char c);
int chcolor(u16 color);

int ps();
int chname();
int kmode();
int tswitch();
int wait();
int kill();

int fork();
int exec();

int pipe();
int read();
int write();
int close();
int pfd();
int test_pipe();


#endif
