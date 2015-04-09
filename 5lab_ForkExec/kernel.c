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
            put_word(0, segment, -i * REG_SIZE);

        put_word(0x0200, segment, UFLAG_FROM_END); // Set flag I bit-1 to allow interrupts 

        // Conform to one-segment model
        put_word(segment, segment, UCS_FROM_END); // Set Umode code  segment 
        put_word(segment, segment, UES_FROM_END); // Set Umode extra segment 
        put_word(segment, segment, UDS_FROM_END); // Set Umode data  segment

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
// BUG: Works with user_two or U2, but u2 is mistaken for u1
int exec(char* cmdline)
{
    int i, length;
    char buf[NAMELEN];
    char *cp = buf;
    u16 usp, segment = running->uss;

    // Get cmdline char by char from user space to kernel space 
    length = 0;
    while(length < NAMELEN)
    {
        *cp = get_byte(segment, (u16)cmdline);
        if(*cp == '\0') break;
        cp++; cmdline++; length++;
    }
    buf[NAMELEN - 1] = '\0'; // ensure it is null terminated
    // buf now contains a local copy of the cmdline

    printf("\nP%d exec '%s' in segment %x\n", 
            running->pid, buf, segment);

    // After loading the new Umode image, fix up the ustack contents to make the
    // process execute from virtual address 0 when it returns to Umode. 
    //          
    //                       For "abc" with length = 3
    //
    // (LOW)              (words)                | by INT 80  |   HIGH
    //  usp  1   2   3   4   5   6   7   8   9  10   11   12  13  14  15  16
    // -----------------------------------------------------------------------
    // |uDS|uES|udi|usi|ubp|udx|ucx|ubx|uax|uPC|uCS|uflag|-4|'a'|'b'|'c'|'\0'|
    // -----------------------------------------------------------------------
    //                                                    -6  -4  -3  -2  -1
    //                                                           (bytes)
    //
    // (a). re-establish ustack to the very high end of the segment.
    // (b). "pretend" it had done  INT 80  from (virtual address) 0: 
    //      (c). fill in uCS, uDS, uES in ustack
    //      (d). execute from VA=0 ==> uPC=0, uFLAG=0x0200, 
    //                                 all other registers = 0;
    //      (e). fix proc.uSP to point at the current ustack TOP (-24)

    // Position of first char: -(length + 1) because (letters + null)
    // Registers are shifted: (length + 1 + 2) because (letters + null + ptr)
    
    // Put cmdline to the High end of the Ustack
    for(i = length; i >= 0; i--)
    {
        put_byte(buf[i], segment, i - (length + 1));
        if(buf[i] == ' ') buf[i] = '\0'; // so only first word gets loaded
    }
    // Point to the cmdline string
    put_word(-(length + 1), segment, -(length + 1 + PTR_SIZE));

    // Load file to beginning of segment
    load(buf, segment);
    //load("/bin/u2", segment);

    // initial USP relative to USS
    running->usp = -((NUM_UREG * REG_SIZE) + (length + 1 + PTR_SIZE)); // Top of Ustack (per INT 80)
    usp = running->usp;

    // Start by clearing all registers
    for(i = 0; i < NUM_UREG; i++)       
        put_word(0, segment, usp + (i * REG_SIZE));

    put_word(0x0200, segment, usp + UFLAG_FROM_USP); // Set flag I bit-1 to allow interrupts 

    // Conform to one-segment model
    put_word(segment, segment, usp + UCS_FROM_USP); // Set Umode code  segment 
    put_word(segment, segment, usp + UES_FROM_USP); // Set Umode extra segment 
    put_word(segment, segment, usp + UDS_FROM_USP); // Set Umode data  segment

    // Now:
    // Flag is set to allow interrupts
    // uCS, uES, uDS are set to the running proc's user mode segment
    // uPC is set to 0, so execution will start from (virtual address) 0 when goUmode
    // The rest of the registers are cleared 

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
