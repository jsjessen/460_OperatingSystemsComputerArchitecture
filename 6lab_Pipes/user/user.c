#include "user.h"

char *cmd[]=
{
    "getpid", 
    "ps", 
    "chname", 
    "kfork", 
    "switch", 
    "wait", 
    "exit", 
    "fork", 
    "exec", 
    "pipe",
    "pfd",
    "close",
    "read",
    "write",
    "test",
    0
};

int show_menu()
{
    printf("********************** Menu ************************\n");
    printf("* ps  chname  kmode switch  wait  exit  fork  exec *\n");
    printf("*  pipe   pfd    close  read  write   test         *\n");
    printf("****************************************************\n");
    return SUCCESS;
}

int find_cmd(char *name)
{
    int i = 0;   
    char *p = cmd[0];

    while(p)
    {
        if (strcmp(p, name) == 0)
            return i;
        i++; 
        p = cmd[i];
    } 
    return FAILURE;
}

int test_pipe()
{
    int pid, n;
    int pd[2];

    // Create pipe
    if((syscall(SYSCALL_PIPE, pd)) != SUCCESS)
        printf("Error user.c: test_pipe(): Create pipe failed");

    // Fork child process
    if((pid = fork()) < 0)
        printf("Error user.c: test_pipe(): Fork failed");

    if(pid)
    { // Parent - Pipe Writer
        //char* buf = "0123456789A0123456789B0123456789C0123456789D0123456789";
        char* buf = "The quick brown fox jumped over the lazy dog!";
        syscall(SYSCALL_CLOSE_PIPE, pd[0]); // close PipeIn
        _getc();
        kswitch();

        n = syscall(SYSCALL_WRITE_PIPE, pd[1], buf, 45);
        printf("Parent P%d wrote %d bytes to the pipe\n", getpid(), n);
        buf[n] = '\0';
        printf("Wrote = \"%s\"\n", buf);
        _getc();

        kswitch();
        return n;
    }
    else
    { // Child - Pipe Reader
        char buf[256];
        syscall(SYSCALL_CLOSE_PIPE, pd[1]); // close PipeOut
        _getc();
        kswitch();
        _getc();

        n = syscall(SYSCALL_READ_PIPE, pd[0], buf, 45);
        printf("Child P%d read %d bytes from the pipe\n", getpid(), n);
        printf("Read = \"%s\"\n", buf);
        _getc();

        return n;
    }
}

int pipe()
{
    int other_pd[2];
    return syscall(SYSCALL_PIPE, other_pd);
}

int pfd()
{
    return syscall(SYSCALL_PFD);
}

int close()
{
    int fd;

    pfd();
    printf("Enter FD to close: ");
    fd = geti();

    return syscall(SYSCALL_CLOSE_PIPE, fd);
}

int read()
{
    int fd, nbytes;
    char buf[1024];

    pfd();
    printf("Read from FD: ");
    fd = geti();
    printf("\nEnter number of bytes to read: ");
    nbytes = geti();
    printf("\n");

    return syscall(SYSCALL_READ_PIPE, fd, buf, nbytes);
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

    return syscall(SYSCALL_WRITE_PIPE, fd, buf, strlen(buf));
}

int getpid()
{
    return syscall(SYSCALL_GET_PID,0,0);
}

int ps()
{
    return syscall(SYSCALL_PS, 0, 0);
}

int chname()
{
    char s[64];
    printf("input new name : ");
    gets(s);
    return syscall(SYSCALL_CHNAME, s, 0);
}

// int kfork()
// {   
//   int child, pid;
//   pid = getpid();
//   printf("proc %d enter kernel to kfork a child\n", pid); 
//   child = syscall(SYSCALL_FORK, 0, 0);
//   printf("proc %d kforked a child %d\n", pid, child);
//   return child;
// }    

int kswitch()
{
    return syscall(SYSCALL_TSWITCH,0,0);
}

int wait()
{
    int child, exitValue;
    printf("proc %d enter Kernel to wait for a child to die\n", getpid());
    child = syscall(SYSCALL_WAIT, &exitValue, 0);
    printf("proc %d back from wait, dead child=%d", getpid(), child);
    if (child>=0)
        printf("exitValue=%d", exitValue);
    printf("\n"); 
    return SUCCESS;
} 

int geti_()
{
    char s[16];
    return atoi(gets(s));
}

int exit()
{
    int exitValue;
    printf("enter an exitValue: ");
    exitValue = geti();
    printf("exitvalue=%d\n", exitValue);
    printf("enter kernel to die with exitValue=%d\n", exitValue);
    return _exit(exitValue);
}

int _exit(int exitValue)
{
    return syscall(SYSCALL_EXIT,exitValue,0);
}


int _getc()
{
    return syscall(SYSCALL_GETC,0,0) & 0x7F;
}

int _putc(char c)
{
    return syscall(SYSCALL_PUTC,c,0,0);
}

int invalid(char *name)
{
    printf("Invalid command : %s\n", name);
    return FAILURE;
}

int fork()
{
    int pid;
    if((pid = syscall(SYSCALL_FORK,0,0)) < 0)
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
    return syscall(SYSCALL_EXEC, cmdline, 0);
}
