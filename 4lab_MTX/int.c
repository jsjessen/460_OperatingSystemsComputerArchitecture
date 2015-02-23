#include "type.h"
#include "wait.c"

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

        case 90: result = getc();       break;
        case 91: result = putc(b);      break;

        case 99: result = kkexit(b);    break;
        default: printf("invalid syscall # : %d\n", a); 
    }
    // Put action function return value in the AX register
    // so that goUmode() can pop it off from Ustack
    put_word(result, segment, offset + AX);
    // return (int) value >= 0 for OK, or -1 for BAD.
}

// return the proc's pid
int kgetpid()
{
    return running->pid;
}

//int color;
extern PROC proc[];

char *hh[ ] = {"FREE   ", "READY  ", "RUNNING", "STOPPED", "SLEEP  ", 
    "ZOMBIE ",  0}; 
// enter Kerenl to print the status info of the procs
// running proc's name to *s.
int do_ps()
{
    int i,j; 
    char *p, *q, buf[16];
    buf[15] = 0;

    printf("============================================\n");
    printf("  name         status      pid       ppid  \n");
    printf("--------------------------------------------\n");

    for (i=0; i<NPROC; i++)
    {
        strcpy(buf,"               ");
        p = proc[i].name;
        j = 0;
        while(*p)
            buf[j] = *p; j++; p++;

        prints(buf);  
        prints(" ");

        if (proc[i].status != FREE)
        {
            if (running==&proc[i])
                prints("running");
            else
                prints(hh[proc[i].status]);
            prints("     ");
            printd(proc[i].pid);  prints("        ");
            printd(proc[i].ppid);
        }
        else
        {
            prints("FREE");
        }
        printf("\n");
    }
    printf("---------------------------------------------\n");

    return(0);
}

// Print PROC information
int kps()
{
    return do_ps();
}

// Change running's name string
#define NAMELEN 32
int kchname(char * y)
{
    char buf[64];
    char *cp = buf;
    int count = 0; 

    while (count < NAMELEN){
        *cp = get_byte(running->uss, y);
        if (*cp == 0) break;
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

int getc()
{
    return 0;
}

int putc(char c)
{
    return 0;
}

        case 90: result = getc();       break;
        case 91: result = putc(b);      break;
