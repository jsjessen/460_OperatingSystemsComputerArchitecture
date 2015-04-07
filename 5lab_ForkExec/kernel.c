#include "kernel.h"


/***********************************************************
  kfork() creates a child proc and returns the child pid.
  When scheduled to run, the child process resumes to body();
************************************************************/
PROC* kfork(char* filename)
{
    int i;
    PROC* p;
    u16 segment;

    //
    if(!filename)
    {
        printf("P%d kfork failed because no file was specified.\n", running->pid);
        return (PROC*)FAILURE;
    }

    // get a first proc from freeList for child process
    if((p = delist(&freeList)) == NULL) 
    {
        printf("Cannot fork because there are no free processes.\n");
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

        put_word(0x0200, segment, UFLAG_FROM_END); // Set flag I bit-1 to allow interrupts 

        // Conform to one-segment model
        put_word(segment, segment, UCS_FROM_END); // Set Umode code  segment 
        put_word(segment, segment, UES_FROM_END); // Set Umode extra segment 
        put_word(segment, segment, UDS_FROM_END); // Set Umode data  segment

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

        printf("P%d kforked child P%d at segment=%x\n",
                running->pid, p->pid, segment);
    }

    return p;
}         

// Fork a child process with an identical Umode image
// Parent returns child pid
// Child  returns 0
// Return -1 if fails
int fork()
{
    int i;
    u16 child_segment;
    PROC* child;
    PROC* parent = running;

    // get a first proc from freeList for child process
    if((child = delist(&freeList)) == NULL)
    {
        printf("Cannot fork because there are no free processes.\n");
        return FAILURE;
    }
    // initialize child process and its stack
    child->status = READY;
    child->priority = 1;
    child->ppid = parent->pid;
    child->parent = parent;

    //---------- Kstack ---------- 

    // clear all saved registers on Kstack
    for (i = 1; i <= NUM_KREG; i++)     // start at 1 to avoid overwriting rPC 
        child->kstack[SSIZE - i] = 0;   // all registers = 0

    // Create a child process such that when 
    // it starts to run (in Kmode), it resumes to Umode
    child->kstack[SSIZE - 1] = (int)goUmode;       
    child->ksp = &(child->kstack[SSIZE - NUM_KREG]); // Save stack top address in proc ksp

    enqueue(&readyQueue, child);
    nproc++;

    // Determine child's segment
    child_segment = 0x1000 * (child->pid + 1);

    printf("P%d forked child P%d at segment=%x\n",
            running->pid, child->pid, child_segment);

    //---------- Ustack ---------- 

    child->usp = parent->usp; 
    child->uss = child_segment;

    // Copy parents segment image to child's segment
    copy_image(running->uss, child_segment);

    // Copied DS, ES, CS = parent_segment
    // Need to use putword to set DS, ES, CS to child_segment 
    // so things don't get messed up when change to Umode
    // Using the one-segment model, so they are all the same segment
    put_word(child_segment, child_segment, child->usp + UCS_FROM_USP); // Set Umode code  segment 
    put_word(child_segment, child_segment, child->usp + UES_FROM_USP); // Set Umode extra segment 
    put_word(child_segment, child_segment, child->usp + UDS_FROM_USP); // Set Umode data  segment

    // Now parent and child are identical except for segment registers (UCS,UES,UDS).
    // Child's stack pointer is the same as parent's, but the usp is just an
    // offset relative to the segment so it doesn't need to be changed.
    // Child thinks its done the same things that parent has done in Umode!

    // Change child's return value to 0
    put_word(0, child_segment, child->usp + UAX_FROM_USP); 

    return child->pid; // Parent returns child's PID
}

// Change the running process's image 
int exec(char* pathname)
{
    int i;
    u16 segment = running->uss;

    printf("\nP%d exec %s in segment %x\n", 
            running->pid, pathname, segment);

    // Load file to beginning of segment
    //load(filename, segment);
    load(pathname, segment);
    // start over from new image

    // -------------------------------------
    // how to pass parameters:
    // pointer to stack
    // if given "a.out one two three"
    // copy whole string to high end of stack
    // main0(char* s) // in crt0.o ?
    // 
    // flag | s | "a.out one two three" |
    //        ^
    //        |
    //        sp
    // 
    // once get string, tokenize using ' ' (space)
    // create an array of pointers pointing to the strings
    // main(int argc, char* argv[])
    // 
    // so main0(), tokenize, then call main() with argc/argv
    // -------------------------------------
    //
    //
        // write 0's to ALL of them
        for(i = 1; i <= NUM_UREG; i++)       
            put_word(0, segment, -i * WORD_SIZE);

        put_word(0x0200, segment, UFLAG_FROM_END); // Set flag I bit-1 to allow interrupts 

        // Conform to one-segment model
        put_word(segment, segment, UCS_FROM_END); // Set Umode code  segment 
        put_word(segment, segment, UES_FROM_END); // Set Umode extra segment 
        put_word(segment, segment, UDS_FROM_END); // Set Umode data  segment

        // execution from uCS segment, uPC offset
        // (segment, 0) = u1's beginning address

        // initial USP relative to USS
        running->usp = -NUM_UREG * WORD_SIZE;  // Top of Ustack (per INT 80)
        running->uss = segment;
    //
    //
    // then move execution to beginning of stack
    // now executing the file

    // (2). After loading the new Umode image, fix up the ustack contents to make the
    //      process execute from virtual address 0 when it returns to Umode. Refer to 
    //      the diagram again:
    // 
    //      0 out di through ax
    //      PC also set to 0
    //      flag = Umode flag
    // 
    //      (LOW)  uSP                                | by INT 80  |   HIGH
    //      ---------------------------------------------------------------------
    //            |uDS|uES| di| si| bp| dx| cx| bx| ax|uPC|uCS|flag| XXXXXX
    //      ---------------------------------------------------------------------
    //             -12 -11 -10  -9  -8  -7  -6  -5  -4  -3  -2  -1 | 0 
    //              0   1   2   3   4    5  6   7   8   9   10  11

    // Zero out PC through DI 
    //for(i = 3; i <= 10; i++)       
    //    put_word(0, segment, -i * WORD_SIZE);

    //put_word(0x0200, segment, -1 * WORD_SIZE); // Set flag I bit-1 to allow interrupts 

    // bp is stack frame pointer, each pointing to another stack frame, linked list ending in 0

    //         (a). re-establish ustack to the very high end of the segment.

    //         (b). "pretend" it had done  INT 80  from (virtual address) 0: 

    //              (d). execute from VA=0 ==> uPC=0, uFLAG=0x0200, 
    //                                         all other registers = 0;
    //
    // write 0's to ALL of them
    //for(i = 1; i <= NUM_UREG; i++)       
    //    put_word(0, segment, -i * WORD_SIZE);

    //put_word(0x0200, segment, UFLAG_FROM_END); // Set flag I bit-1 to allow interrupts 

    ////              (c). fill in uCS, uDS, uES in ustack
    //// Conform to one-segment model
    //put_word(segment, segment, UCS_FROM_END); // Set Umode code  segment 
    //put_word(segment, segment, UES_FROM_END); // Set Umode extra segment 
    //put_word(segment, segment, UDS_FROM_END); // Set Umode data  segment

    //// execution from uCS segment, uPC offset
    //// (segment, 0) = u1's beginning address

    //// initial USP relative to USS
    //running->usp = -NUM_UREG * WORD_SIZE;  // Top of Ustack (per INT 80)



    // Now:
    // Flag is set to allow interrupts
    // uCS, uES, uDS are set to the running proc's user mode segment
    // uPC is set to 0, so execution will start from (virtual address) 0 when goUmode
    // The rest of the registers zeroed

    //              (e). fix proc.uSP to point at the current ustack TOP (-24)
    //running->usp = 
    //      Finally, return from exec() ==> goUmode().

    //child->kstack[SSIZE - 1] = (int)goUmode;       
    //child->ksp = &(child->kstack[SSIZE - NUM_KREG]); // Save stack top address in proc ksp

    // 
    // *************************     BONUS: **************************************
    // 
    //    Implement YOUR exec("filename arg1 arg2 ... argn") in such a way that, 
    // upon entry to the new image, the main() function can be written as
    // 
    //     // main0 tokenize input string into argc/argv and then call main()
    // 
    //     main(int argc, char *argv[ ])
    //     {
    //       // argc = n+1;
    //       // argv[0]= "filename"; argv[1]=arg1, ... argv[n]=argn;
    //     }

    return SUCCESS;
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
