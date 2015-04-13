#include "kernel.h"

// running PROC to sleep on an event value
void ksleep(int event) // consider making this int* to pair with wait better
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
void kwakeup(int event) 
{
    //bool flag = false;
    PROC* p;

    while((p = event_dequeue(&sleepList, event)))
    {
        //if(!flag)
        //    printf("\n");

        color = 0x000A + (p->pid % 6);
        printf("P%d wakes up\n", p->pid);

        p->status = READY;
        enqueue(&readyQueue, p);

        //flag = true;
    }
        color = 0x000A + (running->pid % 6);
}

// to wait for a ZOMBIE child
int kwait(int* status)
{
    int i;
    PROC *p;
    int pid, tmp;
    bool children = false;

    if(!status)
        status = &tmp;

    printf("P%d waits for a child process to die", running->pid); 
    pid = kwait(status);

    if(pid >= 0)
        printf("\n\nP%d finds zombie child P%d with exit status %x and resumes", 
                running->pid, pid, *status);

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
        return FAILURE;
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

                // reset the process
                p->ppid = 0;
                p->parent = NULL;
                p->status = FREE;
                p->priority = 0;
                p->event = 0;
                //p->name[0] = '\0';
                p->exitCode = 0;

                // return it to the free list
                enqueue(&freeList, p);
                return p->pid;
            }
        }
        ksleep((int)running); // sleep at its own &PROC
    }
}
