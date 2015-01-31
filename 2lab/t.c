#include "io.h"

#define NPROC     9        
#define SSIZE  1024                // kstack int size

#define DEAD      0                // proc status   
#define READY     1      

typedef struct proc{
    struct proc *next;   
    int  ksp;               // saved sp; offset = 2 
    int  pid;
    int  status;            // READY|DEAD, etc
    int  kstack[SSIZE];     // kmode stack of task
}PROC;


// #include "io.c" // USE YOUR OWN io.c with YOUR printf() here

//PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
//int procSize = sizeof(PROC);
//int nproc;
//
//int color;

PROC proc[NPROC], *running;
int color;

int  procSize = sizeof(PROC);

// ****************************************************************
//  Initialize the proc's as shown:
//
//  running->proc[0]--> proc[1] --> proc[2] ... --> proc[NPROC-1] -->
//  ^                                         |
//  |<---------------------------------------<------------
//
//  Each proc's kstack contains:
//  retPC, ax, bx, cx, dx, bp, si, di, flag;  all 2 bytes
 // *****************************************************************

int body();  

int initialize()
{
    int i, j;
    PROC *p;

    for (i=0; i < NPROC; i++){
        p = &proc[i];
        p->next = &proc[i+1];
        p->pid = i;
        p->status = READY;

        if (i){     // initialize kstack[ ] of proc[1] to proc[N-1]
            for (j=1; j < 10; j++)
                p->kstack[SSIZE - j] = 0;          // all saved registers = 0
            p->kstack[SSIZE-1]=(int)body;          // called tswitch() from body
            p->ksp = &(p->kstack[SSIZE-9]);        // ksp -> kstack top
        }
    }
    running = &proc[0];
    proc[NPROC-1].next = &proc[0];
    printf("initialization complete\n"); 
}

int body()
{
    //char c;
    //printf("proc %d resumes to body()\n", running->pid);

    //while(1){
    //  printf("-----------------------------------------\n");
    //  printList("freelist  ", freeList);
    //  printQ("readyQueue", readyQueue);
    //  printf("-----------------------------------------\n");

    // printf("proc %d running: priority=%d parent=%d enter a char [s|q|f] : ",
    //         running->pid, running->priority, running->parent->pid );
    //  c = getc(); printf("%c\n", c);
    //  switch(c){
    //     case 'f' : do_kfork();   break;
    //     case 's' : do_tswitch(); break;
    //     case 'q' : do_exit();    break;
    //  }
    //}
    char c;
    printf("proc % resumes to body() function\n");
    while(1){
        printf("I am Proc %d in body(): Enter a key :  ", running->pid);
        c=getc();
        printf("%c\n", c);
        tswitch();
    }
}

main()
{
    //printf("\nMTX starts in main()\n");
    //init();      // initialize and create P0 as running
    //printf("P0 kfork P1\n");

    //kfork();     // P0 kfork() P1
    //while(1){
    //    printf("P0 running\n");
    //    while(!readyQueue);
    //    printf("P0 switch process\n");
    //    tswitch();   // P0 switch to run P1
    //}
    char c;
    printf("\nWelcome to the 460 Multitasking System\n");
    initialize();
    while(1){
        printf("proc %d running : enter a key : ", running->pid);
        c = getc();
        printf("%c\n", c); 
        tswitch();
    }
}

int scheduler()
{
    if (running->status == READY)
        enqueue(&readyQueue, running);

    running = dequeue(&readyQueue);
    color = 0x000A + (running->pid % 6);
}

PROC *kfork()
{
    // YOUR C code to create a CHILD of priority=1, ready to run from body() and
    // enter it into readyQueue by priority.
    return p;       // return child's PROC pointer
}         

int geti()
{
    // YOUR C code to return an integer, e.g. 123
}

int do_tswitch()
{
    printf("proc %d tswitch()\n", running->pid);
    tswitch();
    printf("proc %d resumes\n", running->pid);
}

int do_kfork()
{
    PROC *p;
    printf("proc%d kfork a child\n");
    p = kfork();
    if (p == 0)
        printf("kfork failed\n");
    else
        printf("child pid = %d\n", p->pid);
}

int do_exit()
{
    int exitValue;
    printf("proc %d call kexit() to die\n", running->pid);
    printf("enter a value: ");
    exitValue = geti();
    kexit(exitValue);
}

int kexit(int exitValue)
{
    printf("proc %d in kexit(): exitValue=%d\n", running->pid, exitValue);
    running->exitCode = exitValue;
    running->status = ZOMBIE;
    tswitch();
} 
