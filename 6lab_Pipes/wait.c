#include "kernel.h"

// running PROC to sleep on an event value
void ksleep(int event)
{
    running->event = event;  // Record event in PROC

    // Check if it is already sleeping 
    if(running->status == SLEEPING)
        return;

    running->status = SLEEPING;   // Mark itself as SLEEPING
    enqueue(&sleepList, running); // Using queue for fairness

    tswitch(); 
}

// wakeup ALL PROCs sleeping on event
void kwakeup(int event) // consider making this int* to pair with wait better
{
    bool flag = false;
    PROC* p;

    while((p = event_dequeue(&sleepList, event)))
    {
        if(!flag)
            printf("\n");

        color = 0x000A + (p->pid % 6);
        printf("\nP%d wakes up", p->pid);

        p->status = READY;
        enqueue(&readyQueue, p);

        flag = true;
    }
        color = 0x000A + (running->pid % 6);
}

// to wait for a ZOMBIE child
int kwait(int* status)
{
    int i;
    PROC *p;
    bool children = false;

    // Check if proc has any children
    for (i = 0; i < NPROC; i++)
    {
        p = &proc[i];
        if(p->ppid == running->pid)
        {
            children = true;
            break;
        }
    }

    // If no children, don't wait
    if(!children)
    {
        printf("\nP%d has no children to wait for!\n", running->pid);
        return -1;
    }

    // If children, wait for one to die
    while(true)
    {
        for (i = 0; i < NPROC; i++)
        {
            p = &proc[i];
            if(p->ppid == running->pid && p->status == ZOMBIE)
            {
                *status = p->exitValue;

                // reset the process
                p->ppid = 0;
                p->parent = NULL;
                p->status = FREE;
                p->priority = 0;
                p->event = 0;
                p->name[0] = '\0';
                p->exitValue = 0;

                // return it to the free list
                enqueue(&freeList, p);
                return p->pid;
            }
        }
        ksleep((int)running); // sleep at its own &PROC
    }
}
