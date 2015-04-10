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
    0
};

int show_menu()
{
   printf("********************** Menu ************************\n");
   printf("* ps  chname  kmode switch  wait  exit  fork  exec *\n");
   printf("****************************************************\n");
   return SUCCESS;
}

int find_cmd(char *name)
{
   int i=0;   
   char *p=cmd[0];

   while(p)
   {
         if (strcmp(p, name) == 0)
            return i;
         i++; 
         p = cmd[i];
   } 
   return FAILURE;
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
