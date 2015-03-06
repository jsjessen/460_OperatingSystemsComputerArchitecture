#include "type.h"
#include "lib/io.h"
#include "lib/queue.h"
#include "lib/list.h"
#include "lib/transfer.h"

#define NUM_KREG 9

// Imported from ts.s
int int80h();
int tswitch(void); // does it return an int?

void initialize(void);
int body(void);  
PROC *kfork(char* filename);
void kexit(u16 exitValue);

PROC proc[NPROC], *running, *freeList, *sleepList, *readyQueue;

int procSize = sizeof(PROC);
int nproc = 0; 
int color;

char *pname[] = { "Sun", "Mercury", "Venus", "Earth",  "Mars", 
                  "Jupiter", "Saturn", "Uranus", "Neptune" };

#include "wait.c"
#include "kernel.c"
#include "int.c"

void set_vec(u16 vector, u16 handler)
{
     //      (word, segment, offset)
     put_word(handler, 0x0000,  vector << 2);      // set PC = _int80h
     put_word(0x1000,  0x0000, (vector << 2) + 2); // set CS = 0x1000
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
    int i;
    PROC *p;

    printf("Initializing...");

    // All procs start in freeList
    freeList = &proc[1];
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
    }
    p->next = NULL;

    // P0 starts off running
    running = &proc[0];
    running->status = READY;
    running->parent = &proc[0]; // Parent = self, no parent

    readyQueue = NULL;
    set_vec(80, int80h);

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
            case 'h' : help_menu();    break;

            case 's' : do_tswitch();   break;
            case 'f' : do_kfork("/bin/u1");     break;
            case 'p' : do_ps();        break; 
            case 'z' : do_sleep();     break; 
            case 'a' : do_wakeup();    break; 
            case 'w' : do_wait();      break;
            case 'u' : goUmode();      break;

            case 'q' : do_exit();      break; 
            default  : printf("Unrecognized Command\n");
        }
    }
}

int main()
{
    int value;
    color = 0xB3A;
    printf("MTX starts in main()\n");
    printf("-----------------\n");
    printf("Help Menu: h or ?\n");
    printf("-----------------\n");
    initialize(); // initialize and create P0 as running

    kfork("/bin/u1");  // P0 kfork() P1

    printQueue("readyQueue", readyQueue);
    printf("P%d running\n", running->pid);

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

/***********************************************************
  kfork() creates a child proc and returns the child pid.
  When scheduled to run, the child process resumes to body();
************************************************************/
PROC* kfork(char* filename)
{
    int i;
    PROC* p;
    u16 segment;

    // get a first proc from freeList for child process
    if((p = delist(&freeList)) == NULL) 
    {
        printf("Cannot fork because there are no free processes.\n");
        return (PROC*)FAILURE;
    }

    // initialize new process and its stack
    p->status = READY;
    p->priority = 1;
    p->ppid = running->pid;
    p->parent = running;

    //  Each proc's kstack contains:
    //  retPC, ax, bx, cx, dx, bp, si, di, flag;  all 2 bytes

    // clear all saved registers on Kstack
    for (i = 1; i < NUM_KREG + 1; i++) // start at 1 to skip rPC 
        p->kstack[SSIZE - i] = 0;     // all saved registers = 0

    p->kstack[SSIZE - 1] = (int)body;       // Set rPC so it resumes from body() 
    p->ksp = &(p->kstack[SSIZE - NUM_KREG]); // Save stack top address in proc ksp

    enqueue(&readyQueue, p);
    nproc++;

    if(filename)
    {
        segment = 0x1000 * (p->pid + 1);  // new PROC's segment
        load(filename, segment);          // load file to LOW END of segment

        // SETUP new PROC's ustack for it to return to Umode to execute filename;
        
        //   uSP
        // ---|--------------------------------------------------------
        // | uDS | uES | bp | bx | ax | uPC | uCS | flag | NEXT SEGMENT 
        // ------------------------------------------------------------
        //    -8    -7   -6   -5   -4    -3    -2    -1

#define NUM_UREG 8 
#define REG_SIZE 2

#define UFLAG -1
#define UCS   -2
#define UES   -7
#define UDS   -8

        // write 0's to ALL of them
        for(i = 1; i <= NUM_UREG; i++)       
            put_word(0, segment, -i * REG_SIZE);

        put_word(0x0200, segment, UFLAG * REG_SIZE); // Set flag I bit-1 to allow interrupts 

        // Conform to one-segment model
        put_word(segment, segment, UCS * REG_SIZE); // Set Umode code  segment 
        put_word(segment, segment, UES * REG_SIZE); // Set Umode extra segment 
        put_word(segment, segment, UDS * REG_SIZE); // Set Umode data  segment

        // execution from uCS segment, uPC offset
        // (segment, 0) = u1's beginning address

        // initial USP relative to USS
        p->usp = -NUM_UREG * REG_SIZE;  // Top of Ustack (per INT 80)
        p->uss = segment;

        // When the new PROC execute goUmode in assembly, it does:
        //
        //       restore CPU's ss by PROC.uss ===> segment
        //       restore CPU's sp by PROC.usp ===> sp = usp in the above diagram.
        //
        //       pop registers ===> DS, ES = segment
        //
        //       iret ============> (CS,PC)=(segment, 0) = u1's beginning address.
    }

    printf("P%d kforked child P%d at segment=%x\n",
            running->pid, p->pid, segment);

    return p;
}         

void scheduler()
{
    if(running->status == RUNNING)
        running->status = READY;

    if (running->status == READY)
        enqueue(&readyQueue, running);

    running = dequeue(&readyQueue);
    running->status = RUNNING;

    color = 0x000A + (running->pid % 6);
}

void kexit(u16 exitValue)
{
    int i;
    int count = 0;
    PROC *p;

    // Look for children
    for (i = 0; i < NPROC; i++)
    {
        p = &proc[i];

        // Count active procs while your at it
        if(p->status != ZOMBIE && p->status != FREE)
            count++;

        // Give any orphans to P1
        if(p->ppid == running->pid)
            p->ppid = proc[1].pid;
    }

    // If the dying process is P1
    // Don't let it die unless it is just P0 and P1
    if(running->pid == proc[1].pid && count > 2)
    {
        printf("\nP1 still has children and will never abandon them!\n");
        return;
    }

    running->exitValue = exitValue;
    running->status = ZOMBIE;
    printf("\nP%d stopped: Exit Value = %d", running->pid, exitValue);

    // If parent is sleeping, wake parent 
    if(running->parent->status == SLEEPING)
        kwakeup((int)running->parent);

    // Give up CPU 
    tswitch();
    printf("\nI AM BACK FROM THE DEAD\n");
} 
