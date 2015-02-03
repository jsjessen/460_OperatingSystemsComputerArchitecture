#define NPROC    9
#define SSIZE 1024

/******* PROC status ********/
#define FREE     0
#define READY    1
#define RUNNING  2
#define STOPPED  3
#define SLEEP    4
#define ZOMBIE   5

typedef struct proc{
    struct proc *next;
    int    *ksp;
    int    pid;                // add pid for identify the proc
    int    status;             // status = FREE|READY|RUNNING|SLEEP|ZOMBIE    
    int    ppid;               // parent pid
  struct proc *parent;
    int    priority;
    int    event;
    int    exitCode;
    int    kstack[SSIZE];      // per proc stack area
}PROC;

PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
int procSize = sizeof(PROC);
int nproc = 0;
int color;

#include "wait.c"
#include "kernel.c"

int init()
{
       PROC *p;
       int i;

       printf("init ....");

       for (i=0; i<NPROC; i++){   // initialize all procs
           p = &proc[i];
           p->pid = i;
           p->status = FREE;
           p->priority = 0;     
           p->next = &proc[i+1];
       }
       freeList = &proc[0];      // all procs are in freeList
       proc[NPROC-1].next = 0;
       readyQueue = sleepList = 0;

       /**** create P0 as running ******/
       p = get_proc(&freeList);
       p->status = RUNNING;
       running = p;
       nproc++;
       printf("done\n");
} 

int scheduler()
{
  if (running->status == READY)
      enqueue(&readyQueue, running);
  running = dequeue(&readyQueue);
  color = 0x000A + (running->pid % 6);
}
            
main()
{
    printf("MTX starts in main()\n");
    init();      // initialize and create P0 as running
    kfork();     // P0 kfork() P1
    while(1){
      printf("P0 running\n");
      if (nproc==2 && proc[1].status != READY)
	  printf("no runable process, system halts\n");
      while(!readyQueue);
      printf("P0 switch process\n");
      tswitch();   // P0 switch to run P1
   }
}

int body()
{
  char c;
  printf("proc %d resumes to body()\n", running->pid);
  while(1){
    printf("-----------------------------------------\n");
            //print freeList;
            // print readyQueue;
            // print sleepList;
    printf("-----------------------------------------\n");

    printf("proc %d[%d] running: parent=%d\n",
	   running->pid, running->priority, running->ppid);

    printf("enter a char [s|f|q| p|z|a| w ] : ");
    c = getc(); printf("%c\n", c);
  
    switch(c){
       case 's' : do_tswitch();   break;
       case 'f' : do_kfork();     break;
       case 'q' : do_exit();      break; 
       case 'p' : do_ps();        break; 
       case 'z' : do_sleep();     break; 
       case 'a' : do_wakeup();    break; 
       case 'w' : do_wait();      break;
     }
  }
}

// he uses proc structure address for sleep event
// waiting desposes of 1 dead child if any
// so if 2 dead children, must wait twice to dispose of both

/***********************************************************
  Write YOUR C code for
        ksleep(), kwakeup()
        kexit()
        kwait()

Then, write your C code for
      do_ps(), do_sleep(), do_wakeup(), do_wait()
************************************************************/

// p : print pid, ppid and status of ALL PROCs
ps()
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
