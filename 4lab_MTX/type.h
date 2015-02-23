// James Jessen
// 10918967

#ifndef __TYPE_H__
#define __TYPE_H__


#define NULL      0

#define NPROC     9        
#define NUM_KREG  9
#define NUM_UREG 12 
#define SSIZE  1024                // kstack int size

// Imported from ts.s
int int80h();
void tswitch(void);

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

typedef enum { FREE, READY, RUNNING, STOPPED, SLEEPING, ZOMBIE } status_t;
char* states[] = { "free    ", "ready   ", "running ", "stopped ", "sleeping", "zombie  " };

typedef enum { FAILURE=-1, SUCCESS=0 } result_t;
typedef enum { false, true } bool;

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

PROC proc[NPROC], *running, *freeList, *sleepList, *readyQueue;

int procSize = sizeof(PROC);
int nproc = 0; 
int color;

char *pname[] = { "Sun", "Mercury", "Venus", "Earth",  "Mars", 
                  "Jupiter", "Saturn", "Uranus", "Neptune" };

#endif
