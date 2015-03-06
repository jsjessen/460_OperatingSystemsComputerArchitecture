// ucode.c file

typedef unsigned long  u32;

char *cmd[]={"getpid", "ps", "chname", "kfork", "switch", "wait", "hop", "exit", 0};

#define LEN 64

int show_menu()
{
   printf("***************** Menu ************************\n");
   printf("*  ps  chname  kfork  switch  wait  hop  exit *\n");
   printf("***********************************************\n");
}

int find_cmd(char *name)
{
   int i=0;   
   char *p=cmd[0];

   while (p){
         if (strcmp(p, name)==0)
            return i;
         i++;  
         p = cmd[i];
   } 
   return(-1);
}

int getpid()
{
   return syscall(0,0,0);
}

int ps()
{
   syscall(1, 0, 0);
}

int chname()
{
    char s[64];
    printf("input new name : ");
    gets(s);
    syscall(2, s, 0);
}

int kfork()
{   
  int child, pid;
  pid = getpid();
  printf("proc %d enter kernel to kfork a child\n", pid); 
  child = syscall(3, 0, 0);
  printf("proc %d kforked a child %d\n", pid, child);
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
} 

// Only accurate from -32768 to 32767
int atoi(char* str)
{
    int i;
    int value = 0;
    int len = strlen(str);

    for(i = len; i > 0; i--) 
        value += (str[len - i] - '0') * int_pow(10, i - 1);

    return value;
}

int geti()
{
  char s[16];
  gets(s);

  return atoi(s);
}

int exit()
{
   int exitValue;
   printf("enter an exitValue: ");
   exitValue = geti();
   printf("exitvalue=%d\n", exitValue);
   printf("enter kernel to die with exitValue=%d\n", exitValue);
   _exit(exitValue);
}

int _exit(int exitValue)
{
  syscall(6,exitValue,0);
}


int getc()
{
  return syscall(90,0,0) & 0x7F;
}

int putc(char c)
{
  return syscall(91,c,0,0);
}

int invalid(char *name)
{
    printf("Invalid command : %s\n", name);
}

int hop()
{
    u32 newsegment;
    printf("Hop to segment: ");
    newsegment = geti();
    printf("\n");
    return syscall(7,newsegment,0);
}

