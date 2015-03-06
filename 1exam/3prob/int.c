#include "type.h"

int goUmode();

#define WORD_SIZE 2
extern PROC proc[], *running, *freeList, *sleepList, *readyQueue;
extern int color;

char* states[] = { "free    ", "ready   ", "running ", "stopped ", "sleeping", "zombie  " };

// LOW                                                                       HIGH
//     usp  1   2   3   4   5   6   7   8   9  10    11   12   13  14  15  16
//    -------------------------------------------------------------------------
//    |uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|ucs|uflag|retPC| a | b | c | d |
//    -------------------------------------------------------------------------
// Offset = Position * Size, where Size = 2 bytes or 1 word 
#define PA (13 * 2)
#define PB (14 * 2)
#define PC (15 * 2)
#define PD (16 * 2)
#define AX ( 8 * 2)

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
        case 0 : result = kgetpid();    break;
        case 1 : result = kps();        break;
        case 2 : result = kchname(b);   break;
        case 3 : result = kkfork();     break;
        case 4 : result = ktswitch();   break;
        case 5 : result = kkwait(b);    break;
        case 6 : result = kkexit(b);    break;

        case 7 : result = hop(b);       break;

        case 90: result = kgetc();    break;
        case 91: result = kputc(b);   break;

        case 99: result = kkexit(b);    break;
        default: printf("invalid syscall # : %d\n", a); 
    }
    // Put action function return value in the AX register
    // so that goUmode() can pop it off from Ustack
    put_word(result, segment, offset + AX);
    // return (int) value >= 0 for OK, or -1 for BAD.
}

// copy running segment to a child's segment
// A u16 can store values from 0 to 65,535 
// because it can store 2^16 different values
// so 0 to (2^16)-1
int copy_image(int child_segment)
{
    int i;
    u16 word;

#define WORD_SIZE 2

    // A segment is 64kb
    for(i = 0; i < (64 / WORD_SIZE) * 1024; i++)
    {
        // Consider assuming get/put word is not so simple
        // try using only get/put byte because it was made by KC
        word = get_word(running->uss, i * WORD_SIZE);
        put_word(word, child_segment, i * WORD_SIZE);
    }
    return SUCCESS;
}

int hop(u32 newsegment)
{
    u16 segment;

    printf("Hopping to segment: %d\n", newsegment);

    // Determine running's segment
    segment = 0x1000 * (newsegment + 1);

    copy_image(segment);
    
    // Important: change this AFTER copying
    running->uss = segment;

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
    //          0   1   2   3   4   5   6   7   8   9  10   11

#define REG_SIZE 2

#define UDS 0 
#define UES 1 
#define UCS 10

    // Need to use putword to set DS, ES, CS to the new segment 
    // so things don't get messed up when change to Umode
    // Conform to one-segment model

    put_word(segment, segment, running->usp + (UDS * REG_SIZE); // Set Umode data  segment
    put_word(segment, segment, running->usp + (UES * REG_SIZE); // Set Umode extra segment 
    put_word(segment, segment, running->usp + (UCS * REG_SIZE); // Set Umode code  segment 

    return newsegment;
}

// return the proc's pid
int kgetpid()
{
    return running->pid;
}

// p : print pid, ppid and status of ALL PROCs
void do_ps()
{
    int i,j;
    PROC* p;
    char *cp, buf[16];
    buf[15] = 0;

    printf("\n===========================================\n");
    printf("  Name            Status     PID     PPID  \n");
    printf("-------------------------------------------\n");

    for (i = 0; i < NPROC; i++)
    {
        p = &proc[i];

        strcpy(buf,"               ");
        if(p->name)
        {
            cp = p->name;
            j = 0;
            while(*cp)
                buf[j++] = *(cp++);
        }

        if(p->status == FREE)
            printf("  %s %s\n", buf, states[p->status]);
        else
            printf("  %s %s    %d       %d   \n", 
                    buf, states[p->status], p->pid, p->ppid);
    }
    printf("===========================================\n");
}

// Print PROC information
int kps()
{
    do_ps();
    return 0;
}

// Change running's name string
#define NAMELEN 32
int kchname(char* y)
{
    char buf[64];
    char *cp = buf;
    int count = 0; 

    while (count < NAMELEN)
    {
        *cp = get_byte(running->uss, y);
        if(*cp == 0) break;
        cp++; y++; count++;
    }
    buf[31] = 0;

    printf("changing name of proc %d to %s\n", running->pid, buf);
    strcpy(running->name, buf); 
    printf("done\n");
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
