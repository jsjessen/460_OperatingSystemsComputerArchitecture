// James Jessen
// 10918967

#ifndef __TYPE_H__
#define __TYPE_H__

#define NULL   0

#define SSIZE  1024 // kStack Size in ints
#define PSIZE  10   // Pipe Size
#define NFD    10   // Number of File Descriptors

#define NAMELEN 32  // Max Name Length

typedef unsigned char  u8;  // Unsigned  8-bit = 1 byte
typedef unsigned short u16; // Unsigned 16-bit = 2 bytes
typedef unsigned long  u32; // Unsigned 32-bit = 4 bytes

typedef enum { FAILURE = -1, SUCCESS = 0 } exit_t;
typedef enum { false, true } bool;

typedef struct oft
{
    int    mode;   // READ, WRITE, READ_PIPE, WRITE_PIPE, etc
    int    refCount;
    struct pipe *pipe_ptr;
} OFT;

typedef struct pipe
{
    char   buf[PSIZE];
    int    room; // Empty space in pipe
    int    data; // Number of bytes in pipe, initially 0
    int    head; // In
    int    tail; // Out
    int    nreader; // Number of Readers
    int    nwriter; // Number of Writers
    bool   busy; // True = In Use, False = Free
}PIPE;

typedef struct proc
{
    struct proc *next;    // for creating list/queue

    int*   ksp;           // byte offset 2, Kmode stack pointer 
    int    uss;           // byte offset 4, Umode stack segment 
    int    usp;           // byte offset 6, Umode stack pointer
    int    inkmode;       // byte offset 8  

    char   name[NAMELEN]; // process name string
    int    pid;           // process identity
    int    ppid;          // parent process's identity
    struct proc *parent;  // pointer to parent PROC

    int    status;        // FREE|READY|SLEEP|BLOCK|ZOMBIE
    int    priority;      // scheduling priority

    int    event;         // sleep event
    int    exitValue;     // cause of death

    OFT    *fd[NFD];      // Open File Descriptors

    int    kstack[SSIZE]; // SSIZE=1024 this process's stack
}PROC;


#endif
