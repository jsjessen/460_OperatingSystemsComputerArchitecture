#include "lib/type.h"
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


void do_tswitch()
{
    printf("P%d tswitch()\n\n", running->pid);
    tswitch();
    printf("P%d resumes\n", running->pid);
}

void do_kfork()
{
    PROC *p;
    p = (PROC*)kfork();
}

void do_exit()
{
    printf("P%d stopping...\n", running->pid);
    printf("Enter exit value: ");
    kexit(geti());
}

void do_sleep()
{
    printf("Enter event value to sleep on: ");
    ksleep(geti());
}

// wakeup ALL PROCs sleeping on event
void do_wakeup()
{
    printf("Enter an event value to wakeup: ");
    kwakeup(geti());
}

// to wait for a ZOMBIE child
void do_wait()
{
    int pid, status;
    printf("P%d waits for dead child\n", running->pid); 
    pid = kwait(&status);
    printf("P%d found a ZOMBIE child P%d (exitValue=%d)\n", running->pid, pid, status);
}
