#include "syscall.h"

//==================== Low Level ==================== 

int getpid()
{
    return syscall(GET_PID);
}

int getc()
{
    return syscall(GETC) & 0x7F;
}

int putc(char c)
{
    return syscall(PUTC, c);
}

int chcolor(u16 color)
{
    return syscall(CHCOLOR, color);
}

//==================== Process Basics ==================== 

int ps()
{
    return syscall(PS);
}

int chname()
{
    char s[64];
    printf("input new name : ");
    gets(s);
    printf("\n");
    return syscall(CHNAME, s);
}

int kmode()
{
    syscall(KMODE);
}

int tswitch()
{
    return syscall(TSWITCH);
}

int wait()
{
    int child, exitValue;
    printf("proc %d enter Kernel to wait for a child to die\n", getpid());
    child = syscall(WAIT, &exitValue, 0);
    printf("proc %d back from wait, dead child=%d\n", getpid(), child);
    if (child>=0)
        printf("exitValue=%d", exitValue);
    printf("\n"); 
    return SUCCESS;
} 

int exit()
{
    return syscall(EXIT);
}

//==================== Fork / Exec ==================== 

int fork()
{
    int pid;
    if((pid = syscall(FORK)) < 0)
    {
        printf("Fork Failed\n");
        return FAILURE;
    }

    if(pid)
        printf("Parent returns from fork, child=%d\n", pid); 
    else
        printf("Child returns from fork, child=%d\n", pid);

    return pid;
}

int exec()
{
    char cmdline[64];
    printf("inputs : ");
    gets(cmdline);
    printf("\n");
    return syscall(EXEC, cmdline, 0);
}

//==================== Pipe ==================== 

int pipe()
{
    int pd[2];
    return syscall(PIPE, pd);
}

int pfd()
{
    return syscall(PFD);
}


int read()
{
    int fd, nbytes;
    char buf[1024];

    pfd();
    printf("Read from FD: ");
    fd = geti();
    printf("\n");
    printf("Number of bytes to read: ");
    nbytes = geti();
    printf("\n");

    return syscall(READ_PIPE, fd, buf, nbytes);
}

int write()
{
    int fd;
    char buf[1024];

    pfd();
    printf("Write to FD: ");
    fd = geti();
    printf("\n");
    printf("Write: ");
    gets(buf);
    printf("\n");

    return syscall(WRITE_PIPE, fd, buf, strlen(buf));
}

int close()
{
    int fd;

    pfd();
    printf("Enter FD to close: ");
    fd = geti();
    printf("\n");

    return syscall(CLOSE_PIPE, fd);
}

int test_pipe()
{
    int pid, n;
    int pd[2];

    // Create pipe
    if((syscall(PIPE, pd)) != SUCCESS)
        printf("Create pipe failed");

    // Fork child process
    if((pid = fork()) < 0)
        printf("Fork failed");

    if(pid)
    { // Parent - Pipe Writer
        char* buf = "The quick brown fox jumped over the lazy dog!";
        //char* buf = "0123456789A0123456789B0123456789C0123456789D0123456789";

        syscall(CLOSE_PIPE, pd[0]); // close PipeIn
        kswitch();

        n = syscall(WRITE_PIPE, pd[1], buf, 45);
        buf[n] = '\0';
        _getc();

        syscall(CLOSE_PIPE, pd[1]); // close Pipe

        kswitch();
        return n;
    }
    else
    { // Child - Pipe Reader
        char buf[256];
        syscall(CLOSE_PIPE, pd[1]); // close PipeOut
        kswitch();
        _getc();

        n = syscall(READ_PIPE, pd[0], buf, 45);

        //color = 0x000A + (getpid() % 6); 
        printf("\n(Umode) P%d read = \"%s\"\n", getpid(), buf);

        syscall(CLOSE_PIPE, pd[0]); // close Pipe

        return n;
    }

    pfd();
}
