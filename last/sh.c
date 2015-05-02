// James Jessen
// CptS 460 - Spring 2015

char* thisFileName = "sh.c";

//   (3). then (in sh) it loops forever (until "logout" or Control-D):
//         {
//            prompts for a command line, e.g. cmdLine="cat filename"
//            if(cmd == "logout") 
//               syscall to die;
// 
//            // if just ONE cmd:  
//            pid = fork();
//            if(pid==0)
//                exec(cmdLine);
//            else
//                pid = wait(&status);
//         }    
// 
//    (3). When the child proc terminates (by exit() syscall to MTX kernel), it 
//    wakes up sh, which prompts for another cmdLine, etc.
// 
//    (4). When sh dies, it wakes up its parent, INIT, which forks another
//         login process.         
// 
// ==================================================
// 
// 5-2. Write YOUR OWN sh.c to support I/O redirections and pipes:
// 
//      Examples:  cat [filename]   // NOTE: cat may NOT need a filenmae
//                 cat [filename] >  newfile 
//                 cat [filename] >> appendFile                           
//                 a.out < inFile   // read inputs from inFile 
//                 cat filename | more
//                 cat filename | grep test
//                 cat filename | l2u | grep PRINT
// 
//                 cat filename > /dev/lp0   /* print filename */
//                 cp  filename /dev/lp0     /* print filename */
// 
// 
//                 JJ: cat be careful with "cat f1 > f1" because to display on screen
//                     cat needs to add \r with each \n
//                     but when redirection is used, cat should ommit \r
//                     or else f2 will note the same size (# of char)
// 
//                     does Linux more work with multiple files as input (think mine is supposed to handle that)
// 
// ==========================================================
//                    sh Program:
// YOUR sh must support
// (1). logout/exit :  terminate
// (2). simple command exceution, e.g. ls, cat, .... (anything in the bin dir)
// (3). I/O redirection:  e.g. cat < infile;  cat > outfile, etc. (JJ: file names should be optional)
// (4). (MULTIPLE) PIPEs: e.g. cat file | l2u | grep LINE 
// ===========================================================

#include "ucode.h"
#include "util.h"

int parse(char* line, char* delimiters, char argv[32][256]);
void process_line(char* line);
void io_redirect(char* line);
void do_command(char* line);

char msg_buf[1024];

int main()
{
    char home_dir[32];
    getcwd(home_dir);

    signal(2,1);  // sh ignore Control-C interrupts

    while(true)
    {
        int uid;
        char cwd[256];
        char line[256];
        int argc;
        char argv[32][256];
        int pid, status;

        uid = getuid();
        getcwd(cwd);

        // Get user input 
        do 
        { 
            printf("jjsh:%s$ ", cwd); 
            gets(line);
            argc = parse(line, " ", argv);
        }
        while(argc < 1);

#ifdef DEBUG
        // Show the contents of line, argv, argc
        // {
        //     int i;
        //     printf("Line = \"%s\"\n", line);
        //     for(i = 0; i < argc; i++)
        //         printf("argv[%d] = \"%s\"\n", i, argv[i]);
        //     printf("argc = %d\n\n", argc);
        // }
#endif

        // Logout 
        if(strcmp(argv[0], "logout") == 0 || strcmp(argv[0], "exit") == 0)
            exit(SUCCESS);

        // Change directory (cd)
        if(strcmp(argv[0], "cd") == 0)
        {
            if(argc > 1)
                chdir(argv[1]);
            else if(strcmp(cwd, home_dir) != 0)
                chdir(home_dir);
            continue;
        }

        // Fork child process to execute the command
        if((pid = fork()) < 0)
            perror(thisFileName, "main", "jjsh failed to fork child process");

        if(pid)
        {
            // Parent (sh)
            printf("jjsh waits...\n");
            pid = wait(&status);
            printf("------------------------------------------------------------\n");
            printf("jjsh: My child P%ddied with exit status %x\n", pid, status);
        }
        else
        {
            signal(2,0);  // allow ctrl-C to kill proc 

            // Child (cmd)
            printf("jjsh forks child process %d\n", getpid());
            process_line(line);
        }
    }

    return FAILURE;
}

int parse(char* line, char* delimiters, char argv[32][256])
{
    char tmp[256];
    char* tok;
    int argc = 0;

    strcpy(tmp, line);

    tok = strtok(tmp, " ");
    while(tok)
    {
        strcpy(argv[argc++], tok);
        tok = strtok(NULL, " ");
    }
    return argc;
}


void io_redirect(char* line)
{
    int i, j;
    int argc;
    char argv[32][256];
    char* symbol;
    char* file;

    argc = parse(line, " ", argv);

    // Search for a redirection symbol 
    for(i = 1; i < argc; i++) // argv[0] is cmd, skip it
    {
        symbol = argv[i];
        file = argv[i + 1];

        if(strcmp(symbol, "<") == 0)
        {
#ifdef DEBUG
            sprintf(msg_buf, "Redirecting input from %s\n\r", file);
            print_buf(STDERR, msg_buf);
#endif
            // Replace stdin with file
            close(STDIN); 
            if(open(file, O_RDONLY) < 0)
                perror(thisFileName, "io_redirect", " < open");

            argc = i; // Hide "< file" from executing command 
            break;
        }

        if(strcmp(symbol, ">") == 0)
        {
#ifdef DEBUG
            sprintf(msg_buf, "Redirecting output to %s (overwrite)\n\r", file);
            print_buf(STDERR, msg_buf);
#endif
            // Replace stdout with file (overwrite)
            close(STDOUT);
            if(open(file, O_WRONLY | O_CREAT) < 0)
                perror(thisFileName, "io_redirect", " > open");

            argc = i; // Hide "> file" from executing command 
            break;
        }

        if(strcmp(symbol, ">>") == 0)
        {
#ifdef DEBUG
            sprintf(msg_buf, "Redirecting output to %s (append)\n\r", file);
            print_buf(STDERR, msg_buf);
#endif
            // Replace stdout with file (append)
            close(STDOUT);
            if(open(file, O_WRONLY | O_APPEND | O_CREAT) < 0)
                perror(thisFileName, "io_redirect", " >> open");

            argc = i; // Hide ">> file" from executing command 
            break;
        }
    }

    // Recreate line up to, but not including, redirection symbol
    strcpy(line, argv[0]);
    for(j = 1; j < i; j++)
    {
        strcat(line, " ");
        strcat(line, argv[j]);
    }
}

// Handle Piping
void process_line(char* line)
{
    int pd[2];
    int pid, status;
    int length;
    char* head;
    char* tail;

    if(!line || line[0] == '\0' || strcmp(line, "") == 0)
        exit(SUCCESS);

    // Create a copy of line for this process to use safely
    // line = A | B | C
    // head = A
    // tail = B | C
    length = strlen(line);
    head = strtok(line, "|");

    if(strlen(head) < length)
    {
        // head|tail0
        // .....^
        tail = line + strlen(head) + 1;
    }
    else
    {
        // head0
        // ....^
        tail = line + length;
    }

#ifdef DEBUG
    sprintf(msg_buf, "P%d Pipe: head=%s  tail=%s\n\r", getpid(), head, tail);
    print_buf(STDERR, msg_buf);
#endif

    // If no tail, just do the command in head
    if(!tail || tail[0] == '\0')
        do_command(head); // does not return

    // If reach here, then there was pipe

    // Create Pipe
    if(pipe(pd) < 0)
        perror(thisFileName, "process_line", "creating pipe");

    // Fork child process
    if((pid = fork()) < 0)
        perror(thisFileName, "process_line", "forking child process");

    if(pid == 0)
    { 
        // Child - Pipe Writer
        close(pd[PIPEIN]);// writer MUST close PipeIn 
        close(STDOUT);    // close stdout 
        dup(pd[PIPEOUT]);  // replace stdout with PipeOut 

        // Head is guarenteed to be an atomic cmd, attempt to execute it
        do_command(head);
        exit(FAILURE);
    }
    else
    { 
        // Parent - Pipe Reader
        close(pd[PIPEOUT]); // reader MUST close PipeOut
        close(STDIN);     // close stdin
        dup(pd[PIPEIN]);   // replace stdin with PipeIn

        // Bug Fix
        // =======
        // Do not wait for child/writer!
        // This prevents the back and forth of Pipe Read & Pipe Write.
        // For large files, the pipe's buffer becomes full
        // and everything freezes because there is a DEADLOCK!
        // The Writer is waiting for the Reader to read from the pipe (make room)
        // The Reader is waiting for it's child aka the Writer
        //
        // If the Parent/Reader dies first, don't worry about the shell waking
        // while the Child/Writer is still running because things are already
        // messed up. It would be a BROKEN PIPE situation, where there
        // is a Pipe Writer but no Pipe Reader.

        // Tail could contain more pipes
        // Recurse, only head runs commands
        process_line(tail);
        exit(FAILURE);
    }
}
// ************************************************
//     // line = A | B | C
//     // head = A 
//     // tail = B | C
//     length = strlen(line);
//     head = strtok(line, "|");
//     while(head < tail && *head != '|') { head++; }
// 
//     if(strlen(head) < length)
//     {
//         // head|tail0
//         // .....^
//         tail = line + strlen(head) + 1;
//     }
//     else
//     {
//         // head0
//         // ....^
//         tail = line + length;
//     }
//     
// #ifdef DEBUG
//         sprintf(msg_buf, "P%d Pipe: Head = %s   Tail = %s\n\r", getpid(), head, tail);
//         print_buf(STDERR, msg_buf);
// #endif
// 
//     //if(!tail || tail[0] == '\0')
//     //    do_command(head); // does not return
//     // If reach here, then there was a pipe
// 
//     // Create Pipe
//     if(pipe(pd) < 0)
//         perror(thisFileName, "process_line", "creating pipe");
// 
//     // Fork child process
//     if((pid = fork()) < 0)
//         perror(thisFileName, "process_line", "forking child process");
// 
//     if(pid == 0)
//     { 
//         // Child - Pipe Writer
//         close(pd[PIPEIN]); // Writer must close PipeIn 
//         close(STDOUT);     // Close stdout 
//         dup(pd[PIPEOUT]);  // Replace stdout with PipeOut 
// 
//         // Execute head, it is guarenteed to be a single command 
//         do_command(head);
//         exit(FAILURE);
//     }
//     else
//     { 
//         // Parent - Pipe Reader
//         char* cp;
// 
//         if(!head)
//             exit(SUCCESS);
// 
//         //pid = wait(&status);
//         
//         // Tail could contain more pipes
//         // Recurse, only tail executes commands
//         cp = line + length;
//         while(tail < cp && *cp != '|') { cp--; }
// 
//         if(*cp == '|')
//         {
// #ifdef DEBUG
//             sprintf(msg_buf, "P%d Process Line: %s\n\r", getpid(), head);
//             print_buf(STDERR, msg_buf);
// #endif
//             close(pd[PIPEOUT]); // Reader must close PipeOut
//             close(STDIN);       // Close stdin
//             dup(pd[PIPEIN]);    // Replace stdin with PipeIn
// 
//             process_line(tail);
//         }
//         else
//             do_command(tail);
// 
//         exit(FAILURE);
//     }
// }

void do_command(char* cmd)
{
    if(!cmd || cmd[0] == '\0')
        perror(thisFileName, "do_command", "Input argument 'cmd' is empty");

    // Check for IO redirection (<, >, >>)
    io_redirect(cmd);

#ifdef DEBUG
    sprintf(msg_buf, "P%d Execute: %s\n\r", getpid(), cmd);
    print_buf(STDERR, msg_buf);

    sprintf(msg_buf, "------------------------------------------------------------\n\r");
    print_buf(STDERR, msg_buf);
#endif

    // Execute the command
    exec(cmd);

    // Getting here implies execution failed
    sprintf(msg_buf, "Invalid Command: %s\n\r", cmd);
    print_buf(STDERR, msg_buf);
    exit(FAILURE);
}
