// James Jessen
// CptS 460 - Spring 2015

//   (1). P2 is a login process. It executes /bin/login on the console terminal 
//        /dev/tty0. So the special file /dev/tty0 MUST exist. To support serial 
//        terminals and printer, /dev/ttyS0, /dev/ttyS1 and /dev/lp0 must also 
//        exist.
// 
//   (2). In /bin/login, P2 opens its tty special file (/dev/tty0) as stdin(0), 
//        stdout(1) and stderr(2). Then it displays (to its stdout)
//                   login: 
// 
//   and waits for a user to login. When a user tries to login, it reads the
//   user name and password (from its stdin), opens the /etc/passwd file to 
//   authenticate the user. Each line of /etc/passwd has the format:
// 
//           username:password:gid:uid:fullname:HOMEDIR:program
//   e.g.    root:xxxxxxx:1000:0:superuser:/root:sh
// 
//   (Use plain text for password OR devise your own encryption schemes)
// 
//   If the user has a valid account in /etc/passwd, P2 becomes the user's process
//   (by taking on the user's uid). It chdir to user's HOMEDIR and execute the 
//   listed program, which is normally the sh (/bin/sh).
//  
// ==========================================================================

#include "ucode.h"
#include "util.h"

char *tty;
char* password_file = "/etc/passwd";
int stdin, stdout, stderr;

typedef struct user
{
    char name[32];
    char password[32];
    int gid;
    int uid;
    char fullname[64];
    char home[256];
    char program[256];
}USER;

bool verify(USER* user);

int main(int argc, char *argv[])   // invoked by exec("login /dev/ttyxx")
{
    tty = argv[1];

    // Close anything you may of inherited from your parent
    close(0); close(1); close(2); // login process may run on different terms

    // Open own tty as stdin, stdout, stderr in order to display messages 
    stdin = open(tty, O_RDONLY);
    stdout = open(tty, O_WRONLY);
    stderr = open(tty, O_WRONLY); 

    if(stdin < 0 || stdout < 0 || stderr < 0)
    {
        // Try to print error, might not be able to see it
        printf("Error login: Failed to open stdin/stdout/stderr\n");
        exit(FAILURE);
    }

    settty(tty);  // store tty string in PROC.tty[] for putc()
    signal(2,1);  // ignore Control-C interrupts so that 
    // Control-C KILLs other procs on this tty but not the main sh

    // NOW we can use printf, which calls putc() to our tty
    printf("JJLOGIN : Open %s as stdin, stdout, stderr\n", tty);

    printf("=========\n");
    printf("* LOGIN *\n");
    printf("=========\n");

    while(true)
    {
        USER user;

#ifdef DEBUG
        // {
        //     user.gid = 4;
        //     user.uid = 4;
        //     strcpy(user.fullname, "James Jessen");
        //     //strcpy(user.home, "/user/james");
        //     strcpy(user.home, "/");
        //     strcpy(user.program, "sh");

        //     chuid(user.uid, user.gid);
        //     chdir(user.home);
        //     exec(user.program);
        // }
#endif

        printf("Username: ");
        gets(user.name);

        printf("Password: ");
        gets(user.password);

        if(verify(&user))
        {
            printf("JJLOGIN : Welcome %s!\n", user.name);
            printf("JJLOGIN : gid=%d uid=%d home=%s  program=%s\n",
                    user.gid, user.uid, user.home, user.program);

            // Change to user account and execute start program
            chuid(user.uid, user.gid);
            chdir(user.home);
            exec(user.program);
        }
        else
        {
            printf("+++++++++++++++++++++++++++\n");
            printf(" Login failed - Try again! \n");
            printf("+++++++++++++++++++++++++++\n");
        }
    }
    return 0;
}

// Verify user name and password from /etc/passwd file
bool verify(USER* user)
{
    char line[256];
    int nbytes = 256 - 1;
    int bytes_read;
    int fd; 

    if((fd= open(password_file, O_RDONLY)) < 0)
    {
        printf("Error login->verify: Unable to open %s\n", password_file);
        return false;
    }

    do 
    {
        bytes_read = getline(line, nbytes, fd);
        line[bytes_read - 1] = '\0'; // overwrite '\n' with null term

        // username:password:gid:uid:fullname:homeDir:startProgram
        // root:12345:0:0:super user:/:sh

        if(strcmp(user->name, strtok(line, ":")) == 0 &&
                strcmp(user->password, strtok(NULL, ":")) == 0)
        {
            user->gid = atoi(strtok(NULL, ":"));
            user->uid = atoi(strtok(NULL, ":"));
            strcpy(user->fullname, strtok(NULL, ":"));
            strcpy(user->home, strtok(NULL, ":"));
            strcpy(user->program, strtok(NULL, ":"));
            return true;
        }
    } 
    while(bytes_read > 0);

    return false;
}
