#include "io.h"
#include "type.h"
#include "queue.h"
#include "list.h"

PROC proc[NPROC], *running, *freeList, *sleepList, *readyQueue;
int procSize = sizeof(PROC);
int nproc; 

int color;

int body(void);  
PROC* delist(PROC** list);
void enlist(PROC** list, PROC* new);
PROC* kfork(void);
void enqueue(PROC **queue, PROC *p);
int do_tswitch(void);
void do_kfork(void);
void do_exit(void);
void printQueue(char* name, PROC* queue);
void printList(char* name, PROC* list);

u16 pow(u16 base, u16 power)
{
    u16 i;
    u16 result;

    result = base;

    if(base < 0 || power < 0)
        return 0;

    if(power == 0)
        return 1;

    for(i = 1; i < power; i++)
        result *= base;

    return result;
}

u16 geti()
{
    char str[32];
    u16 result = 0;
    u16 len = 0;
    u16 i;

    gets(str);
    while(str[len])  
        len++; 

    for(i = len; i > 0; i--) 
        result += (str[len - i] - '0') * pow(DEC, i - 1);

    return result;
}
void initialize()
{
    int i;
    PROC *p;

    printf("Initializing...");

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
        printList ("freeList  ", freeList);
        printQueue("readyQueue", readyQueue);
        printf("-----------------------------------------------------------------------\n\n");

        printf("P%d running: priority=%d parent=%d enter a char [s|q|f] : ",
                running->pid, running->priority, running->parent->pid );

        c = getc(); 
        printf("%c\n", c);

        switch(c)
        {
            case 'f' : do_kfork();   break; 
            case 's' : do_tswitch(); break;
            case 'q' : do_exit();    break; 
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
            printf("P0 switch process\n");
            tswitch();   // P0 switch to run P1
        }
    }
}

PROC* kfork()
{
    int i;
    PROC* p = delist(&freeList); // get a FREE proc from freeList

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

int do_tswitch()
{
    printf("P%d tswitch()\n", running->pid);
    tswitch();
    printf("P%d resumes\n", running->pid);

    return 0;
}

void do_kfork()
{
    PROC *p;
    p = (PROC*)kfork();
}

void kexit(u16 exitValue)
{
    printf("\nP%d stopped: Exit Value = %d\n", running->pid, exitValue);
    running->exitCode = exitValue;
    running->status = ZOMBIE;
    tswitch();
} 

void do_exit()
{
    printf("P%d stopping...\n", running->pid);
    printf("Enter exit value: ");
    kexit(geti());
}
