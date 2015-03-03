#include "kernel.h"

#define NAMELEN 32

// LOW                                                                       HIGH
//     usp  1   2   3   4   5   6   7   8   9  10    11   12   13  14  15  16
//    -------------------------------------------------------------------------
//    |uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|ucs|uflag|retPC| a | b | c | d |
//    -------------------------------------------------------------------------
// Offset = Position * Size, where Size = 2 bytes or 1 word 
#define  PA (13 * WORD_SIZE)
#define  PB (14 * WORD_SIZE)
#define  PC (15 * WORD_SIZE)
#define  PD (16 * WORD_SIZE)

#define UCS (10 * WORD_SIZE)
#define UAX ( 8 * WORD_SIZE)
#define UES ( 1 * WORD_SIZE)
#define UDS ( 0 * WORD_SIZE)

// ****************** syscall handler in C ******************
int kcinth()
{
    u16 segment, offset;
    int a,b,c,d, result;
    segment = running->uss; 
    offset = running->usp;

    // Get syscall parameters from Ustack
    a = get_word(segment, offset + PA);
    b = get_word(segment, offset + PB);
    c = get_word(segment, offset + PC);
    d = get_word(segment, offset + PD);

    // Parameter 'a' is the syscall number
    switch(a)
    {
        // Execute the desired action function
        case 0 : result = kgetpid();            break;
        case 1 : result = kps();                break;
        case 2 : result = kchname((char*)b);    break;
        case 3 : result = kkfork();             break;
        case 4 : result = ktswitch();           break;
        case 5 : result = kkwait((int*)b);      break;
        case 6 : result = kkexit(b);            break;

        case 7 : result = fork();               break;
        case 8 : result = exec((char*)b);       break;

        case 90: result = kgetc();              break;
        case 91: result = kputc((char)b);       break;

        case 99: result = kkexit(b);            break;

        default: 
                 printf("invalid syscall # : %d\n", a); 
                 result = FAILURE;
    }
    // Put action function return value in the AX register
    // so that goUmode() can pop it off from Ustack
    put_word(result, segment, offset + UAX);
    // return (int) value >= 0 for OK, or -1 for BAD.

    return SUCCESS;
}

// return the proc's pid
int kgetpid()
{
    return running->pid;
}


// Print PROC information
int kps()
{
    do_ps();
    return 0;
}

// Change running's name string
int kchname(char* name)
{
    char buf[NAMELEN];
    char *cp = buf;
    int count = 0; 

    while(count < NAMELEN)
    {
        *cp = get_byte(running->uss, (u16)name);
        if(*cp == '\0') break;
        cp++; name++; count++;
    }
    buf[NAMELEN - 1] = '\0';

    printf("changing name of proc %d to %s\n", running->pid, buf);
    strcpy(running->name, buf); 
    printf("done\n");
    return SUCCESS;
}

// enter Kernel to kfork a child with /bin/u1 as Umode image
// return child pid or -1 
int kkfork()
{
    return do_kfork("/bin/u1");
}
// enter Kernel to switch process (call tswitch())
int ktswitch()
{
    return tswitch();
}

// enter Kernel to wait for a ZOMBIE child,
// return its pid (or -1 if no child) and its exitValue
int kkwait(int *status)
{
    return kwait(status);
}

// enter Kernel to die with an exitValue
int kkexit(int value)
{
    // do NOT let P1 die
    return kexit(value);
}

int kgetc()
{
    return getc();
}

int kputc(char c)
{
    return putc(c);
}

// copy running segment to a child's segment
// A u16 can store values from 0 to 65,535 
// because it can store 2^16 different values
// so 0 to (2^16)-1
int copy_image(int child_segment)
{
    u16 i;
    u16 word;

    if(running->uss != (running->pid + 1) * 0x1000)
        printf("running->uss != (running->pid + 1) * 0x1000\n");
    else
        printf("running->uss == (running->pid + 1) * 0x1000\n");
        

    // A segment is 64kb
    //for(i = 0; i < (64 / WORD_SIZE) * 1024; i++)
    for(i = 0; i < 32768; i++)
    {
        // Consider assuming get/put word is not so simple
        // try using only get/put byte because it was made by KC
        word = get_word(running->uss, i * WORD_SIZE);
        put_word(word, child_segment, i * WORD_SIZE);
    }
    return SUCCESS;
}

int cmp_image(int segA, int segB)
{


}

// fork - running process creates a child process
// parent returns child pid
// child  returns 0
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
    for (i = 1; i < NUM_KREG + 1; i++) // start at 1 becuase first array index is 0
        child->kstack[SSIZE - i] = 0;  // all registers = 0

    // Create a child process such that when 
    // it starts to run (in Kmode), it resumes to goUmode()
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
    copy_image(child_segment);

    //        Assume: P1 fork() P2 by        int r = syscall(7,0,0);
    // 		      
    //                              Parent (running)
    // ========================== P1's image in 0x2000 ============================
    // 
    // 0x2000                   |       int80h             |  INT 80  |syscall(7,0,0)
    // ----------------------------------------------------|--------------------------
    // |Code Data               |DS|ES|di|si|bp|dx|cx|bx|ax|PC|CS|flag|rPC|7|0|0|xxxx| 
    // --------------------------|-----------------------------------------------|----
    //                          usp                                              usp
    // P1's PROC.uss=0x2000      |             DS,ES,CS = 0x2000
    //           usp= ------------         
    // 
    //                              Child of running
    // ===========================P2's image in 0x3000 ============================
    // 
    // 0x3000                   |       int80h             |  INT 80  |syscall(7,0,0)
    // ----------------------------------------------------|--------------------------
    // |Code Data               |DS|ES|di|si|bp|dx|cx|bx|ax|PC|CS|flag|rPC|7|0||xxxxx| 
    // --------------------------|----------------------------------------------------
    //                          usp    
    // P2's PROC.uss=0x3000      |      COPIED DS,ES,CS = 0x2000 ALSO
    //           usp=-------------                        but needs to be 0x3000
    // 
    // PC should be the same (offset in segment)

    // Need to use putword to set DS, ES, CS to 0x3000 
    // so things don't get messed up when change to Umode
    // Conform to one-segment model
    put_word(child_segment, child_segment, child->usp + UCS); // Set Umode code  segment 
    put_word(child_segment, child_segment, child->usp + UES); // Set Umode extra segment 
    put_word(child_segment, child_segment, child->usp + UDS); // Set Umode data  segment

    // Now parent and child are identical except for segment registers (UCS,UES,UDS).
    // Child's stack pointer is the same as parent's, but the usp is just an
    // offset relative to the segment so it doesn't need to be changed.
    // Child thinks its done the same things that parent has done in Umode.
    
    if(child->usp != running->usp)
        printf("child->usp != running->usp\n");
    else
        printf("child->usp == running->usp\n");

    // Change child's return value to 0
    put_word(0, child_segment, child->usp + UAX); // try with running->usp (should be same)
    return child->pid; // Parent returns child's PID
}

// a process can use exec to change image
int exec(char* filename)
{
    int i;
    char buf[NAMELEN];
    char *cp = buf;
    int count = 0; 
    u16 segment = running->uss;

    // Get filename char by char from user space to kernel space 
    while(count < NAMELEN)
    {
        *cp = get_byte(segment, (u16)filename);
        if(*cp == '\0') break;
        cp++; filename++; count++;
    }
    buf[NAMELEN - 1] = '\0';
    // buf now contains a local copy of the filename

    // Locate the file "filename", verify it's eXecutable, read file header
    // to determine the TOTAL memory needed, allocate a memory area for the 
    // NEW Umode image, then load the EXECUTABLE part of the file into memory.
    // 
    // The load(filename, segment) function is given. Use it as is.

    // load file to beginning of segment
    load(buf, segment);
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
    // 
    for(i = 1; i <= NUM_UREG; i++)       
        put_word(0, segment, -i * WORD_SIZE);

    put_word(0x0200, segment, -1 * WORD_SIZE); // Set flag I bit-1 to allow interrupts 


    // bp is stack frame pointer, each pointing to another stack frame, linked list ending in 0
    //         (a). re-establish ustack to the very high end of the segment.
    //         (b). "pretend" it had done  INT 80  from (virtual address) 0: 
    //              (c). fill in uCS, uDS, uES in ustack
    
    // Conform to one-segment model
    put_word(segment, segment,  -2 * WORD_SIZE); // Set Umode code  segment 
    put_word(segment, segment, -11 * WORD_SIZE); // Set Umode extra segment 
    put_word(segment, segment, -12 * WORD_SIZE); // Set Umode data  segment

    //              (d). execute from VA=0 ==> uPC=0, uFLAG=0x0200, 
    //                                         all other registers = 0;
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
