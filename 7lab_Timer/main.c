#include "kernel.h"

PROC proc[NPROC], *running, *freeList, *sleepList, *readyQueue;

int procSize = sizeof(PROC);
int nproc = 0; 
int color;

char *pname[] = { "Sun", "Mercury", "Venus", "Earth",  "Mars", 
                  "Jupiter", "Saturn", "Uranus", "Neptune" };

OFT oft[NOFT];
PIPE pipe[NPIPE];

void set_vec(u16 vector, u16 handler)
{
    u16 location = vector << 2;

    put_word(handler, 0,  location);    // set PC = _int80h
    put_word(0x1000,  0, location + 2); // set CS = 0x1000
}

void help_menu()
{
    printf("=====================================================\n");
    printf(" u : Enter user mode\n"); 
    printf(" p : Print the pid, ppid, and status of all processes\n");
    printf(" f : Fork a new child process\n");
    printf(" s : Switch to another process \n"); 
    printf(" z : Sleep until a specific event\n");
    printf(" a : Wake all processes sleeping the event\n");
    printf(" w : Wait for a zombie child process\n");
    printf(" q : Kill the current process\n");
    printf("=====================================================\n");
}


void initialize()
{
    int i, j;
    PROC *p;

    printf("Initializing...");

    // All procs start in freeList
    freeList = proc;
    readyQueue = NULL;
    sleepList = NULL;

    for (i = 0; i < NPROC; i++)
    {
        p = &proc[i];

        p->next = &proc[i + 1];
        p->pid = i;
        p->ppid = 0;
        p->parent = NULL;
        p->status = FREE;
        p->priority = 0;
        p->event = 0;
        strcpy(p->name, pname[i]);
        p->exitValue = 0;

        // Clear fd[ ] array of PROC
        for(j = 0; j < NFD; j++)
            p->fd[j] = 0;
    }
    p->next = NULL;

    // Initialize all OFT and PIPE structures    
    for(i = 0; i < NOFT; i++)
        oft[i].refCount = 0;
    for(i = 0; i < NPIPE; i++)
        pipe[i].busy = false;

    // P0 starts off running
    running = delist(&freeList);
    running->status = RUNNING;
    running->ppid   = running->pid;
    running->parent = running; // Parent = self, no parent
    nproc = 1;

    set_vec(80, (u16)int80h);

    printf("ok\n"); 
}

int body()
{
    char c;
    printf("\n\nP%d resumes to body()", running->pid);

    while(true)
    {
        printf("\n-----------------------------------------------------------------------\n");
        printList (" freeList  ", freeList);
        printQueue(" readyQueue", readyQueue);
        printList (" sleepList ", sleepList);
        printf("-----------------------------------------------------------------------\n");

        printf("P%d (priority=%d parent=%d) : ", running->pid, running->priority, running->parent->pid );

        c = getc(); 
        printf("%c\n", c);

        switch(c)
        {
            case '?' :
            case 'h' : help_menu();      break;

            case 's' : do_tswitch();     break;
            case 'f' : kfork("/bin/u1"); break;
            case 'p' : do_ps();          break; 
            case 'z' : do_sleep();       break; 
            case 'a' : do_wakeup();      break; 
            case 'w' : do_wait(NULL);    break;
            case 'u' : goUmode();        break;
            case 'q' : do_exit();        break; 

            default  : printf("Unrecognized Command\n");
        }
    }
}

int main()
{
    color = 0xB3A;
    printf("MTX starts in main()\n");
    printf("-----------------\n");
    printf("Help Menu: h or ?\n");
    printf("-----------------\n");
    initialize(); // initialize and create P0 as running

    kfork("/bin/u1");  // P0 kfork() P1

    printQueue("readyQueue", readyQueue);
    printf("P%d running...\n", running->pid);

    while(true)
    {
        if(readyQueue)
        {
            printf("P0 switch process");
            tswitch();   // P0 switch to run P1
            printf("\n\nP%d running...\n", running->pid);
        }
    }
}

void scheduler()
{
    if(running->status == RUNNING)
        running->status = READY;

    if(running->status == READY)
        enqueue(&readyQueue, running);

    running = dequeue(&readyQueue);
    running->status = RUNNING;

    color = 0x000A + (running->pid % 6);
}

