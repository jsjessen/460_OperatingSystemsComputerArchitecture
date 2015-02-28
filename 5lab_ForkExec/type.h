// James Jessen
// 10918967

#ifndef __TYPE_H__
#define __TYPE_H__

#define NULL 0
#define SSIZE  1024  // kstack int size

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

typedef struct proc
{
    struct proc *next;   // for creating list/queue

    int*   ksp;          // byte offset 2, Kmode stack pointer 
    int    uss;          // byte offset 4, Umode stack segment 
    int    usp;          // byte offset 6, Umode stack pointer
    int    inkmode;      // byte offset 8  

    int    pid;          // process id
    int    ppid;         // parent pid 
    struct proc *parent; // pointer to parent PROC

    int    status;       // FREE|READY|SLEEP|BLOCK|ZOMBIE
    int    priority;     // scheduling priority

    int    event;        // sleep event
    char   name[32];     // process name string
    int    exitValue;    // cause of death

    int    kstack[SSIZE]; // SSIZE=1024 this process's stack
}PROC;


#endif
