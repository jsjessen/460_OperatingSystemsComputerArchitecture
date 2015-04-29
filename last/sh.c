//   (3). then (in sh) it loops forever (until "logout" or Contro-D):
//         {
//            prompts for a command line, e.g. cmdLine="cat filename"
//            if (cmd == "logout") 
//               syscall to die;
// 
//            // if just ONE cmd:  
//            pid = fork();
//            if (pid==0)
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
