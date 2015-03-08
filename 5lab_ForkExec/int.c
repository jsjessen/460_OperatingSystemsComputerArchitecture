#include "kernel.h"

// LOW                                                                       HIGH
//     usp  1   2   3   4   5   6   7   8   9  10    11   12   13  14  15  16
//    -------------------------------------------------------------------------
//    |uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|ucs|uflag|retPC| a | b | c | d |
//    -------------------------------------------------------------------------
// Offset = Position * Size, where Size = 2 bytes or 1 word 

// relative to usp 
#define  PA_FROM_USP (13 * WORD_SIZE)
#define  PB_FROM_USP (14 * WORD_SIZE)
#define  PC_FROM_USP (15 * WORD_SIZE)
#define  PD_FROM_USP (16 * WORD_SIZE)

// ****************** syscall handler in C ******************
int kcinth()
{
    u16 segment, offset;
    int a,b,c,d, result;
    segment = running->uss; 
    offset = running->usp;

    // Get syscall parameters from Ustack
    a = get_word(segment, offset + PA_FROM_USP);
    b = get_word(segment, offset + PB_FROM_USP);
    c = get_word(segment, offset + PC_FROM_USP);
    d = get_word(segment, offset + PD_FROM_USP);

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
        case 8 : result = kexec((char*)b);      break;

        case 90: result = kgetc();              break;
        case 91: result = kputc((char)b);       break;

        case 99: result = kkexit(b);            break;

        default: 
                 printf("invalid syscall # : %d\n", a); 
                 result = FAILURE;
    }
    // Put action function return value in the AX register
    // so that goUmode() can pop it off from Ustack
    put_word(result, segment, offset + UAX_FROM_USP);
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

int kexec(char* filename)
{
    return do_exec(filename);
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

