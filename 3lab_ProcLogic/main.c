#include "type.h"
#include "lib/io.h"
#include "lib/queue.h"
#include "lib/list.h"

void initialize(void);
int body(void);  
PROC* kfork(void);
void kexit(u16 exitValue);

#include "wait.c"
#include "kernel.c"

void help_menu()
{
    printf("------------\n");
    printf(" Switch : s\n"); 
    printf(" Fork   : f\n");
    printf(" Print  : p\n");
    printf(" Sleep  : z\n");
    printf(" Wake   : a\n");
    printf(" Wait   : w\n");
    printf(" Quit   : q\n");
    printf("------------\n");
}

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

        printf("\nP%d (priority=%d parent=%d) : ", running->pid, running->priority, running->parent->pid );

        c = getc(); 
        printf("%c\n", c);

        switch(c)
        {
            case '?' :
            case 'h' : help_menu();    break;

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
    PROC* p = delist(&freeList); // get a first proc from freeList

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

/***********************************************************
  Write YOUR C code for
  ksleep(), kwakeup()
  kexit()
  kwait()

  Then, write your C code for
  do_ps(), do_sleep(), do_wakeup(), do_wait()
 ************************************************************/

void kexit(u16 exitValue)
{
    int i;
    int count = 0;
    PROC *p;

    // Look for children
    for (i = 0; i < NPROC; i++)
    {
        p = &proc[i];

        // Count active procs while your at it
        if(p->status != ZOMBIE && p->status != FREE)
            count++;

        // Give any children to P1
        if(p->ppid == running->pid)
            p->ppid = proc[1].pid;
    }

    // If the dying process is P1
    // Don't let it die unless it is just P0 and P1
    if(running->pid == proc[1].pid && count > 2)
    {
        printf("\nP1 still has children and refuses to die!\n");
        return;
    }

    // If parent is sleeping, wake parent 
    if(running->parent->status == SLEEPING)
        kwakeup((int)running->parent);

    printf("\nP%d stopped: Exit Value = %d\n\n", running->pid, exitValue);
    running->exitCode = exitValue;
    running->status = ZOMBIE;
    tswitch();
} 
