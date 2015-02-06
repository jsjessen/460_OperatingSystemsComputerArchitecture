#include "type.h"
#include "lib/io.h"
#include "lib/queue.h"
#include "lib/list.h"

PROC proc[NPROC], *running, *freeList, *sleepList, *readyQueue;

int procSize = sizeof(PROC);
int nproc = 0; 
int color;

void initialize(void);
int body(void);  
PROC* kfork(void);
void kexit(u16 exitValue);

#include "wait.c"
#include "kernel.c"

void initialize()
{
    int i;
    PROC *p;

    printf("Initializing...");

    // All procs start in freeList
    freeList = &proc[1];
    for (i = 0; i < NPROC; i++)
    {
        p = &proc[i];
        p->priority = 0;
        p->pid = i;
        p->ppid = 0;
        p->parent = NULL;
        p->status = FREE;
        p->next = &proc[i + 1];
    }
    p->next = NULL;

    // P0 starts off running
    running = &proc[0];
    running->status = READY;
    running->parent = &proc[0]; // Parent = self, no parent

    readyQueue = NULL;

    printf("ok\n"); 
}

int body()
{
    char c;
    printf("P%d resumes to body()\n", running->pid);

    while(true)
    {
        printf("-----------------------------------------------------------------------\n");
        printList (" freeList  ", freeList);
        printQueue(" readyQueue", readyQueue);
        printf("-----------------------------------------------------------------------\n");

        printf("\nP%d running: priority=%d parent=%d enter a char [s|f|q| p|z|a| w ] : ",
                running->pid, running->priority, running->parent->pid );

        c = getc(); 
        printf("%c\n", c);

        switch(c)
        {
            case 's' : do_tswitch();   break;
            case 'f' : do_kfork();     break;
            case 'q' : do_exit();      break; 
            case 'p' : do_ps();        break; 
            case 'z' : do_sleep();     break; 
            case 'a' : do_wakeup();    break; 
            case 'w' : do_wait();      break;

            default  : printf("Unrecognized Command\n");
        }
    }
}

int main()
{
    color = 0xB3A;
    printf("\nMTX starts in main()\n");
    initialize();      // initialize and create P0 as running

    kfork(); // P0 kfork() P1
    printQueue("readyQueue", readyQueue);
    printf("P%d running\n", running->pid);

    while(true)
    {
        if(readyQueue)
        {
            printf("P0 switch process\n\n");
            tswitch();   // P0 switch to run P1
            printf("P%d running\n", running->pid);
        }
    }
}

PROC* kfork()
{
    int i;
    PROC* p = delist(&freeList); // get a FREE proc from freeList

    if(!p)
    {
        printf("Cannot fork because there are no free processes.\n");
        return NULL;
    }

    printf("P%d forks child P%d\n", running->pid, p->pid);
    p->status = READY;
    p->priority = 1;
    p->ppid = running->pid;
    p->parent = running;

    //    INITIALIZE p's kstack to make it start from body() when it runs.

    //    To do this, PRETEND that the process called tswitch() from the 
    //    the entry address of body() and executed the SAVE part of tswitch()
    //    to give up CPU before. 
    //    Initialize its kstack[ ] and ksp to comform to these.

    //  Each proc's kstack contains:
    //  retPC, ax, bx, cx, dx, bp, si, di, flag;  all 2 bytes

    for (i = 1; i < NUM_REG + 1; i++) // start at 1 becuase first array index is 0
        p->kstack[SSIZE - i] = 0;         // all saved registers = 0

    // Empty stack, so ksp points at very bottom of stack
    p->kstack[SSIZE - 1] = (int)body; // called tswitch() from body, set rPC
    p->ksp = &(p->kstack[SSIZE - 9]); // ksp -> kstack top

    enqueue(&readyQueue, p);
    return p;
}         

void scheduler()
{
    if (running->status == READY)
        enqueue(&readyQueue, running);

    running = dequeue(&readyQueue);
    color = 0x000A + (running->pid % 6);
}

// he uses proc structure address for sleep event
// waiting desposes of 1 dead child if any
// so if 2 dead children, must wait twice to dispose of both

/***********************************************************
  Write YOUR C code for
  ksleep(), kwakeup()
  kexit()
  kwait()

  Then, write your C code for
  do_ps(), do_sleep(), do_wakeup(), do_wait()
 ************************************************************/

// p : print pid, ppid and status of ALL PROCs
void ps()
{
    int i;
    PROC* p;

    printf("PID  PPID  STATUS\n");
    printf("---  ----  ------\n");

    for (i = 0; i < NPROC; i++)
    {
        p = &proc[i];

        if(p->status == FREE)
            printf("        %d\n", p->status);
        else
            printf("%d  %d  %d\n", p->pid, p->ppid, p->status);
    }
}

void kexit(u16 exitValue)
{
    printf("\nP%d stopped: Exit Value = %d\n\n", running->pid, exitValue);
    running->exitCode = exitValue;
    running->status = ZOMBIE;
    tswitch();
} 
