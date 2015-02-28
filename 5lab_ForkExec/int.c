#include "kernel.h"

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
        case 0 : result = kgetpid();            break;
        case 1 : result = kps();                break;
        case 2 : result = kchname((char*)b);    break;
        case 3 : result = kkfork();             break;
        case 4 : result = ktswitch();           break;
        case 5 : result = kkwait((int*)b);      break;
        case 6 : result = kkexit(b);            break;

        case 90: result = kgetc();              break;
        case 91: result = kputc((char)b);       break;

        case 99: result = kkexit(b);            break;

        default: 
                 printf("invalid syscall # : %d\n", a); 
                 result = FAILURE;
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
        *cp = get_byte(running->uss, (u16)y);
        if(*cp == 0) break;
        cp++; y++; count++;
    }
    buf[31] = 0;

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
