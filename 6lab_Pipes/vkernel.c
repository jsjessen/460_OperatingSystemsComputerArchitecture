#include "kernel.h"
#include "string.h"

char* states[] = { "free    ", "ready   ", "running ", "stopped ", "sleeping", "zombie  " };

// p : print pid, ppid and status of ALL PROCs
void do_ps()
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
}

void do_tswitch() // s
{
    printf("P%d tswitch()", running->pid);
    tswitch();
    printf("\n\nP%d resumes", running->pid);
}

int do_kfork(char* filename) // f
{
    return (int)kfork(filename);
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

// to wait for a ZOMBIE child
void do_wait() // w
{
    int pid, status;
    printf("P%d waits for a child process to die", running->pid); 
    pid = kwait(&status);

    if(pid >= 0)
        printf("\n\nP%d finds zombie child P%d with exit status %x and resumes", running->pid, pid, status);
}

int do_exec(char* cmdline)
{
    return exec(cmdline);
}
