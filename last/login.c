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

char *tty;
int stdin, stdout, stderr;

#include "ucode.c"

// parent is init() on console
int main(int argc, char *argv[])   // invoked by exec("login /dev/ttyxx")
{
    tty = argv[1];

    // JJ: close anything you may of inherited from your parent
    close(0); close(1); close(2); // login process may run on different terms

    // Open own tty as stdin, stdout, stderr in order to display messages 
    stdin = open(tty, O_RDONLY);
    stdout = open(tty, O_WRONLY); // open("/dev/tty0", O_WRONLY)
    stderr = open(tty, O_WRONLY); 

    // JJ: Now we have our own 0, 1, 2 and can printf
    //     every process must remember which tty (terminal) it is running on
    //     printf shouldn't be showing up on another guys terminal!

    settty(tty);  // store tty string in PROC.tty[] for putc()

    // NOW we can use printf, which calls putc() to our tty
    printf("KCLOGIN : open %s as stdin, stdout, stderr\n", tty);

    signal(2,1);  // ignore Control-C interrupts so that 
    // Control-C KILLs other procs on this tty but not the main sh

    while(1)
    {
        // 1. show login:           to stdout
        printf("Login: ");
        // 2. read user name        from stdin
        // 3. show passwd:
        // 4. read user passwd

        // 5. verify user name and passwd from /etc/passwd file
        // JJ: open the file in read-only mode
        //     tokenize the different fields
        //     strcmp  name with name, and pword with pword

        // 6. if (user account valid){
        //    setuid to user uid. (think he said setgid is done at the same time as setuid)
        //    chdir to user HOME directory.
        //    exec to the program in users's account
        //    }
        printf("Login failed, try again\n");
    }
}
