#include "kernel.h"

// LOW                                                                  HIGH
// -------------------------------------------------------------------------
// |uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|ucs|uflag|retPC| a | b | c | d |
// -------------------------------------------------------------------------
//  usp  1   2   3   4   5   6   7   8   9  10    11   12   13  14  15  16
#define PA_FROM_USP (13 * REG_SIZE)
#define PB_FROM_USP (14 * REG_SIZE)
#define PC_FROM_USP (15 * REG_SIZE)
#define PD_FROM_USP (16 * REG_SIZE)

// ****************** syscall handler in C ******************
int kcinth()
{
    int a,b,c,d;
    int result = FAILURE;

    u16 segment = running->uss; 
    u16 offset = running->usp;

    // Get syscall parameters from Ustack
    a = get_word(segment, offset + PA_FROM_USP); // syscall #
    b = get_word(segment, offset + PB_FROM_USP); // arg1
    c = get_word(segment, offset + PC_FROM_USP); // arg2
    d = get_word(segment, offset + PD_FROM_USP); // arg3

    switch(a)  
    {
        case  0 : result = running->pid;               break;
        case  1 : result = do_ps();                    break;
        case  2 : result = chname((char*)b);           break;
        case  3 : result = kmode();                    break;
        case  4 : result = tswitch();                  break;
        case  5 : result = do_wait((int*)b);           break;
        case  6 : result = fork();                     break;
        case  7 : result = exec((char*)b);             break;

        // Pipe Functions
        case 30 : result = kpipe((int*)b);             break;
        case 31 : result = read_pipe(b, (char*)c, d);  break;
        case 32 : result = write_pipe(b, (char*)c, d); break;
        case 33 : result = close_pipe(b);              break;
        case 34 : result = pfd();                      break;

        case 90 : result = getc();                     break;
        case 91 : color  = running->pid + 11;
                  result = putc(b);                    break;       

        case 99 : do_exit(b);                          break;

        default : printf("Invalid syscall # : %d\n", a); 
                  result = FAILURE;
    }

    // Put action function return value in the AX register
    // so that goUmode() can pop it off from Ustack
    put_word(result, segment, offset + UAX_FROM_USP);

    return SUCCESS;
}

// Change running's name string
int chname(char* name)
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

int kmode()
{
    return body();
}

// enter Kernel to die with an exitValue
int kkexit(int value)
{
    // do NOT let P1 die
    return kexit(value);
}
