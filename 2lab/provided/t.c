/************ t.c file **********************************/
#define NPROC     9        
#define SSIZE  1024                /* kstack int size */

#define DEAD      0                /* proc status     */
#define READY     1      

typedef struct proc{
    struct proc *next;   
           int  ksp;               /* saved sp; offset = 2 */
           int  pid;
           int  status;            /* READY|DEAD, etc */
           int  kstack[SSIZE];     // kmode stack of task
}PROC;


// #include "io.c" /**** USE YOUR OWN io.c with YOUR printf() here *****/

PROC proc[NPROC], *running;

int  procSize = sizeof(PROC);

/****************************************************************
 Initialize the proc's as shown:

 running->proc[0]--> proc[1] --> proc[2] ... --> proc[NPROC-1] -->
                       ^                                         |
            |<---------------------------------------<------------

 Each proc's kstack contains:
      retPC, ax, bx, cx, dx, bp, si, di, flag;  all 2 bytes
*****************************************************************/

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
    running = running->next;
}

