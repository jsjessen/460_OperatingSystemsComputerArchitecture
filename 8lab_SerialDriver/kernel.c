#include "kernel.h"
#include "string.h"

// p : print pid, ppid and status of ALL PROCs
int kps()
{
    int i,j;
    PROC* p;
    char *cp, buf[16];
    buf[15] = 0;

    printf("\n===========================================\n");
    printf("  Name            Status     PID     PPID  \n");
    printf("-------------------------------------------\n");

    for (i = 0; i < NPROC; i++)
    {
        p = &proc[i];

        strcpy(buf,"               ");
        if(p->name)
        {
            cp = p->name;
            j = 0;
            while(*cp)
                buf[j++] = *(cp++);
        }

        if(p->status == FREE)
            printf("  %s %s\n", buf, states[p->status]);
        else
            printf("  %s %s    %d       %d   \n", 
                    buf, states[p->status], p->pid, p->ppid);
    }
    printf("===========================================\n");

    return SUCCESS;
}

void do_tswitch() // s
{
    printf("P%d tswitch()", running->pid);
    tswitch();
    printf("\n\nP%d resumes", running->pid);
}

int do_exit() // q
{
    printf("Enter exit value: ");
    return kexit(geti());
}

void do_sleep() // z
{
    int event = 0;
    while(event <= 0)
    {
        printf("Enter event value to sleep on: ");
        event = geti();

        if(event <= 0)
            printf("\nEvent value must be greater than zero\n\n");
    }

    ksleep(event);
}

// wakeup ALL PROCs sleeping on event
void do_wakeup() // a
{
    printf("Enter an event value to wakeup: ");
    kwakeup(geti());
}
int kmode()
{
    printf("\nkmode has not been implemented yet\n");
    return SUCCESS;
}

int kexit(u16 exitCode)
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

        // Give any orphans to P1
        if(p->ppid == running->pid)
            p->ppid = proc[1].pid;
    }

    // If the dying process is P1
    // Don't let it die unless it is just P0 and P1
    if(running->pid == proc[1].pid && count > 2)
    {
        printf("\nP1 still has children and will never abandon them!\n");
        return FAILURE;
    }

    running->exitCode = exitCode;
    running->status = ZOMBIE;
    printf("\nP%d stopped: Exit Value = %d\n", running->pid, exitCode);

    // If parent is sleeping, wake parent 
    if(running->parent->status == SLEEPING)
        kwakeup((int)running->parent);

    // Close opened file descriptors
    for(i = 0; i < NFD; i++)
    {
        if(running->fd[i] != 0)
            close_pipe(i);
    }

    tswitch(); // Give up CPU 
    printf("\nI AM BACK FROM THE DEAD\n");
    return SUCCESS;
} 
