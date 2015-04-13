#include "lib/io.h"
#include "type.h"

void do_tswitch() // s
{
    printf("P%d tswitch()", running->pid);
    tswitch();
    printf("\n\nP%d resumes", running->pid);
}

void do_kfork() // f
{
    PROC *p;
    p = (PROC*)kfork();
}

void do_exit() // q
{
    printf("Enter exit value: ");
    kexit(geti());
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

// to wait for a ZOMBIE child
void do_wait() // w
{
    int pid, status;
    printf("P%d waits for a child process to die", running->pid); 
    pid = kwait(&status);

    if(pid >= 0)
        printf("\n\nP%d finds zombie child P%d with exit status %x and resumes", running->pid, pid, status);
}

// p : print pid, ppid and status of ALL PROCs
void do_ps()
{
    int i;
    PROC* p;

    printf("======================================\n");
    printf("  Name       Status     PID     PPID  \n");
    printf("--------------------------------------\n");

    for (i = 0; i < NPROC; i++)
    {
        p = &proc[i];

        if(p->status == FREE)
            printf("             %s\n", states[p->status]);
        else
            printf("  %s           %s    %d       %d   \n", 
                    p->name, states[p->status], p->pid, p->ppid);
    }
    printf("======================================\n");
}
