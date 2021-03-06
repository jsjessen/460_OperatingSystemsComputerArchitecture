#include "type.h"

PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
int procSize = sizeof(PROC);
int nproc = 0;

int body();
char *pname[]={"Sun", "Mercury", "Venus", "Earth",  "Mars", "Jupiter", 
               "Saturn", "Uranus", "Neptune" };
OFT  oft[NOFT];
PIPE pipe[NPIPE];

/**************************************************
  bio.o, queue.o loader.o are in mtxlib
**************************************************/
#include "wait.c"
#include "kernel.c"
#include "int.c"
#include "fe.c"
#include "pipe.c"

int init()
{
    PROC *p;
    int i, j;
    printf("init ....");
    for (i=0; i<NPROC; i++){   // initialize all procs
        p = &proc[i];
        p->pid = i;
        p->status = FREE;
        p->priority = 0;  
        strcpy(proc[i].name, pname[i]);
        p->next = &proc[i+1];

        //NEW
        // clear fd[ ] array of PROC
        for (j=0; j<NFD; j++)
             p->fd[j] = 0;
    }
    freeList = &proc[0];      // all procs are in freeList
    proc[NPROC-1].next = 0;
    readyQueue = sleepList = 0;

    // initialize all OFT and PIPE structures    
    for (i=0; i<NOFT; i++)
        oft[i].refCount = 0;
    for (i=0; i<NPIPE; i++)
        pipe[i].busy = 0;

    /**** create P0 as running ******/
    p = get_proc(&freeList);
    p->status = READY;
    p->ppid   = 0;
    p->parent = p;
    running = p;
    nproc = 1;
    printf("done\n");
} 

int scheduler()
{
    if (running->status == READY){
       enqueue(&readyQueue, running);
    }
    printList("readyQueue", readyQueue);
    running = dequeue(&readyQueue);
}

int int80h();

int set_vector(u16 vector, u16 addr)
{
    u16 location,cs;
    location = vector << 2;
    put_word(addr, 0, location);
    put_word(0x1000,0,location+2);
}
            
main()
{
    printf("MTX starts in main()\n");
    init();      // initialize and create P0 as running
    set_vector(80,int80h);

    kfork("/bin/u1");     // P0 kfork() P1

    while(1){
      printf("P0 running\n");
      if (nproc==2 && proc[1].status != READY)
	  printf("no runable process, system halts\n");
      while(!readyQueue);
      printf("P0 switch process\n");
      tswitch();   // P0 switch to run P1
   }
}
