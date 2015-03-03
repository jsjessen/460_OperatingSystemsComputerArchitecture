#include "kernel.h"

#define UFLAG (-1  * WORD_SIZE) // Flag
#define UCS   (-2  * WORD_SIZE) // Code Segment
#define UES   (-11 * WORD_SIZE) // Extra Segment
#define UDS   (-12 * WORD_SIZE) // Data Segment

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
    for (i = 1; i < NUM_KREG + 1; i++) // start at 1 becuase first array index is 0
        p->kstack[SSIZE - i] = 0;     // all saved registers = 0


    p->kstack[SSIZE - 1] = (int)goUmode;     // Set rPC so it resumes from body() 
    p->ksp = &(p->kstack[SSIZE - NUM_KREG]); // Save stack top address in proc ksp

    enqueue(&readyQueue, p);
    nproc++;

    if(filename)
    {
        segment = 0x1000 * (p->pid + 1);  // new PROC's segment
        load(filename, segment);          // load file to LOW END of segment

        //       new PROC
        //        ------
        //        |uss | = new PROC's SEGMENT value
        //        |usp | = -24                                    
        //        --|---                                    Assume P1 in segment=0x2000:
        //          |                                              0x30000  
        //          |                                              HIGH END of segment
        //  (LOW) | |   ----- by SAVE in int80h ----- | by INT 80  |
        //  --------|-------------------------------------------------------------
        //        |uDS|uES| di| si| bp| dx| cx| bx| ax|uPC|uCS|flag|NEXT segment
        //  ----------------------------------------------------------------------
        //         -12 -11 -10  -9  -8  -7  -6  -5  -4  -3  -2  -1 |
        //

        // SETUP new PROC's ustack for it to return to Umode to execute filename;

        // write 0's to ALL of them
        for(i = 1; i <= NUM_UREG; i++)       
            put_word(0, segment, -i * WORD_SIZE);

        put_word(0x0200, segment, UFLAG); // Set flag I bit-1 to allow interrupts 

        // Conform to one-segment model
        put_word(segment, segment, UCS); // Set Umode code  segment 
        put_word(segment, segment, UES); // Set Umode extra segment 
        put_word(segment, segment, UDS); // Set Umode data  segment

        // execution from uCS segment, uPC offset
        // (segment, 0) = u1's beginning address

        // initial USP relative to USS
        p->usp = -NUM_UREG * WORD_SIZE;  // Top of Ustack (per INT 80)
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

int kexit(u16 exitValue)
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
        return FAILURE;
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
    return SUCCESS;
} 
