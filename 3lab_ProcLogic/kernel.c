#include "lib/io.h"
#include "type.h"

void do_tswitch() // s
{
    printf("P%d tswitch()\n\n", running->pid);
    tswitch();
    printf("P%d resumes\n", running->pid);
}

void do_kfork() // f
{
    PROC *p;
    p = (PROC*)kfork();
}

void do_exit() // q
{
    printf("P%d stopping...\n", running->pid);
    printf("Enter exit value: ");
    kexit(geti());
}

void do_sleep() // z
{
    printf("Enter event value to sleep on: ");
    ksleep(geti());
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
    printf("P%d waits for dead child\n", running->pid); 
    pid = kwait(&status);
    printf("P%d found a ZOMBIE child P%d (exitValue=%d)\n", running->pid, pid, status);
}

// p : print pid, ppid and status of ALL PROCs
void do_ps()
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
