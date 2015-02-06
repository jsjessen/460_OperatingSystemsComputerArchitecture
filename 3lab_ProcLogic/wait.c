#include "type.h"
#include "lib/queue.h" 
#include "lib/list.h" 

// running PROC to sleep on an event value
void ksleep(int event)
{
    running->event = event;  // Record event in PROC

    if(running->status == SLEEPING)
        return;
    else
    {
        running->status = SLEEPING;  // Mark itself as SLEEPING
        enlist(&sleepList, running); // Consider changing sleepList to a queue
    }

    tswitch(); 
}

// wakeup ALL PROCs sleeping on event
void kwakeup(int event)
{
    PROC* p = sleepList;

    while(p)
    {
        // If event matches, move from sleepList to readyQueue
        if(p->event == event)
            enqueue(&readyQueue, delist(&sleepList, p));
    }
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
            children = true;
    }

    // If no children, don't wait
    if(!children)
    {
        printf("P%d has no children, so will not wait.\n", running->pid);
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
                *status = p->exitCode;
                enlist(&freeList, p);
                return p->pid;
            }
        }
        ksleep((int)running); // sleep at its own &PROC
    }
}

