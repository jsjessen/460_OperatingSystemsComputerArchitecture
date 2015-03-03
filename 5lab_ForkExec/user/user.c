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
   return syscall(0,0,0);
}

int ps()
{
   return syscall(1, 0, 0);
}

int chname()
{
    char s[64];
    printf("input new name : ");
    gets(s);
    return syscall(2, s, 0);
}

int kfork()
{   
  int child, pid;
  pid = getpid();
  printf("proc %d enter kernel to kfork a child\n", pid); 
  child = syscall(3, 0, 0);
  printf("proc %d kforked a child %d\n", pid, child);
  return child;
}    

int kswitch()
{
    return syscall(4,0,0);
}

int wait()
{
    int child, exitValue;
    printf("proc %d enter Kernel to wait for a child to die\n", getpid());
    child = syscall(5, &exitValue, 0);
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
  return syscall(6,exitValue,0);
}


int _getc()
{
  return syscall(90,0,0) & 0x7F;
}

int _putc(char c)
{
  return syscall(91,c,0,0);
}

int invalid(char *name)
{
    printf("Invalid command : %s\n", name);
    return FAILURE;
}

int fork()
{
    int pid;
    if((pid = syscall(7,0,0)) < 0)
    {
        printf("Fork Failed\n");
        return FAILURE;
    }

    if(pid)
        printf("parent return from fork, child=%d\n", pid); 
    else
        printf("child return from fork, child=%d\n", pid);

    return pid;
}

int exec()
{
    char filename[64];
    printf("input new name : ");
    gets(filename);
    return syscall(8, filename, 0);
}
