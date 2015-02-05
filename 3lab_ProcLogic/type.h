// James Jessen
// 10918967

#ifndef __TYPE_H__
#define __TYPE_H__


#define NULL      0

#define NPROC     9        
#define NUM_REG   9
#define SSIZE  1024                // kstack int size

void tswitch(void);

typedef enum { FREE, READY, RUNNING, STOPPED, SLEEP, ZOMBIE } status_t;
typedef enum { FAILURE, SUCCESS } result_t;
typedef enum { false, true } bool;

typedef struct proc
{
    struct proc *next;   // for linked list
    int*   ksp;          // saved sp; offset = 2 

    int    status;       // FREE|READY|SLEEP|BLOCK|ZOMBIE
    int    priority;     // priority
    int    event;
    int    exitCode;
    int    pid;          // process pid
    int    ppid;         // parent pid 
    struct proc *parent; // pointer to parent PROC

    int    kstack[SSIZE]; // SSIZE=1024 This processes stack
}PROC;


#endif
