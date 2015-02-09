// James Jessen
// 10918967

#ifndef __TYPE_H__
#define __TYPE_H__


#define NULL      0

#define NPROC     9        
#define NUM_REG   9
#define SSIZE  1024                // kstack int size

void tswitch(void);

typedef enum { FREE, READY, RUNNING, STOPPED, SLEEPING, ZOMBIE } status_t;
char* states[] = { "free    ", "ready   ", "running ", "stopped ", "sleeping", "zombie  " };

typedef enum { FAILURE, SUCCESS } result_t;
typedef enum { false, true } bool;

typedef struct proc
{
    struct proc *next;   // for linked list
    int*   ksp;          // saved sp; offset = 2 bytes

    int    uss, usp;     // Add items at BYTE offset 4, 6, and
    int    inkmode;      // inkmode at BYTE offset 8

    int    pid;          // process id
    int    ppid;         // parent pid 
    struct proc *parent; // pointer to parent PROC

    int    status;       // FREE|READY|SLEEP|BLOCK|ZOMBIE
    int    priority;     // scheduling priority

    int    event;        // sleep event
    char   name[32];     // name string of proc
    int    exitValue;

    int    kstack[SSIZE]; // SSIZE=1024 This process's stack
}PROC;

PROC proc[NPROC], *running, *freeList, *sleepList, *readyQueue;

int procSize = sizeof(PROC);
int nproc = 0; 
int color;


#endif
