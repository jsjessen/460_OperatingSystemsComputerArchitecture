// James Jessen
// CptS 460 - Spring 2015

//   After mounting the root file system, P0 creats P1, whose Umode image is 
//   the /bin/init program. P1 will go Umode directly to execute /bin/init, 
//   in which it forks children P2 on /dev/ttyS0. If the VM supports serial ports,
//   P1 may also fork P3 /dev/ttyS0 and P4 on /dev/ttyS1. Then P1 waits for any 
//   child to die. Henceforth, P1 plays the role of the INIT process (P1) in Unix.

#include "ucode.h"

int stdin, stdout, stderr;

int init(char* name);
void login(char* path);

int main(int argc, char *argv[])
{
    int pid_console = -1; 
    int pid_serial0 = -1; 
    int pid_serial1 = -1; 

    // 1. open /dev/tty0 as 0 (READ) and 1 (WRITE) in order to display console messages
    stdin = open("/dev/tty0", O_RDONLY);
    stdout = open("/dev/tty0", O_WRONLY);
    stderr = open("/dev/tty0", O_WRONLY);

    if(stdin < 0 || stdout < 0 || stderr < 0)
    {
        // Try to print error, might not be able to see it
        printf("Error init: Failed to open stdin/stdout/stderr\n");
        exit(FAILURE);
    }

    // 2. Now we can use printf, which calls putc(), which writes to stdout
    printf("--------------------------------------------------\n");
    printf("JJINIT P%d: Starting initialization on console\n", getpid()); 

    pid_serial0 = init("ttyS0");
    pid_serial1 = init("ttyS1");
    pid_console = init("tty0"); // Last so prints don't invade input space

    // P1 should loop forever, waiting for one of its children to die
    // It can then clean up and replace the child process 
    while(true)
    {
        int pid, status;

        printf("JJINIT P%d: Waiting...\n", getpid());
        printf("--------------------------------------------------\n");
        pid = wait(&status);

        printf("\n--------------------------------------------------\n");
        printf("JJINIT P%d: My child P%dhas died with status %x\n", 
                getpid(), pid, status);

        if(pid == pid_console)
            pid_console = init("tty0");
        else if(pid == pid_serial0)
            pid_serial0 = init("ttyS0");
        else if(pid == pid_serial1)
            pid_serial1 = init("ttyS1");
        else
        {
            printf("JJINIT P%d: I didn't know I had that child!\n", getpid());
        }
    }
}       

// Update stdin/stdout then login
// Example name = "tty0"
int init(char* name)
{
    char path[128];
    int pid;

    pid = fork();

    if(pid)
    {
        printf("JJINIT P%d: Forking login task P%don %s\n", getpid(), pid, name); 
        return pid;
    }

    // Close old stdin/stdout
    if(stdin >= 0) close(stdin);
    if(stdout >= 0) close(stdout);
    if(stderr >= 0) close(stdout);

    // Open new stdin/stdout
    strcpy(path, "/dev/");
    strcat(path, name);
    stdin = open(path, O_RDONLY);
    stdout = open(path, O_WRONLY); // open("/dev/tty0", O_WRONLY)
    stderr = open(path, O_WRONLY); 

    if(stdin < 0 || stdout < 0 || stderr < 0)
    {
        // Try to print error, might not be able to see it
        printf("Error init: Failed to open stdin/stdout/stderr\n");
        exit(FAILURE);
    }

    printf("JJINIT P%d: Starting login on %s\n", getpid(), name); 
    login(path); // Should never return
}

// Run login command on path
// Never return
// Example name = "/dev/tty0"
void login(char* path)
{
    char buf[128];

    strcpy(buf, "login ");
    strcat(buf, path);
    exec(buf); // exec("login /dev/tty0")

    printf("Execution of login failed!\n");
    exit(FAILURE);
}
