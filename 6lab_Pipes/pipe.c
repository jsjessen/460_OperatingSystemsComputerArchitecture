#include "kernel.h"
#include "fcntl.h" // Open()

char *MODE[] = {"READ_PIPE ", "WRITE_PIPE"};

void show_pipe(PIPE *p)
{
// displays the contents of the pipe, e.g.
//       ------------------------------------------------------   
//        #data = number of chars currently in the pipe;
//        #room = number of rooms available in the pipe;
//        contents = "........."; (actual chars in the pipe).
//       ------------------------------------------------------
    //int i, j;

    printf("------------ PIPE CONTENETS ------------\n");     
    // print pipe information
    printf("\n----------------------------------------\n");
}


int pfd()
{
    // print running process' opened file descriptors
    return 0;
}
//============================================================

int read_pipe(int fd, char *buf, int n)
{
// (B). READER Process: call  
//       
//              n = read(pd[0], buf, nbytes);
// 
//       which tries to read nbytes from the pipe, subject to these constraints:
//       ---------------------------------------------------------------------
//       (1). If (no WRITER on the pipe){
//               read as much as it can; either nbytes or until no more data.
//               return ACTUAL number of bytes read
//            }.
//            --------------------------------------------------------------
//                       (pipe still have WRITERs)
//       (2). if (pipe has data){
//               read until nbytes or (3).
//               "wakeup" WRITERs that are waiting for room
//            }
//       (3). if (NO data in pipe){
//               "wakeup" WRITERs that are waiting for room
//               "wait" for data; 
//               then try to read again from (1).
//            }
// 
//      ---------------------------------------------- 
//      JJ: dec ref count by one then follow, dec reader by one (remember iput)
//      kclose()  ===> close a file descriptor ==> special handling of pipes.
//      ===================================================================
// Inside the read_pipe()/write_pipe() function, it would be very informative 
// if you call showPipe() to display the pipe conditions and contents.

    // your code for read_pipe()
    return 0;
}

int write_pipe(int fd, char *buf, int n)
{
// (A). WRITER Proces: call 
// 
//             n = write(pd[1], buf, nbytes);
// 
//      which tries to write nbytes of data to the pipe, subject to these 
//      constraints.
//           ------------------------------------------------------------------
//      (1). If (no READER on the pipe) return BROKEN_PIPE_ERROR;
//           ------------------------------------------------------------------
//                      (pipe still have READERs):
//      (2). If (pipe has room){
//               write as much as it can until all nbytes are written or (3).
//               "wakeup" READERs that are waiting for data.  
//           }
//      (3)  If (no room in pipe){
//              "wakeup" READERs that are waiting for data
//              "wait" for room; 
//               then try to write again from (1).
//           }
//           ------------------------------------------------------------------
// Inside the read_pipe()/write_pipe() function, it would be very informative 
// if you call showPipe() to display the pipe conditions and contents.
    OFT *op; 
    PIPE *pp;

    printf("P%d write_pipe: fd=%d\n", running->pid, fd);

    op = running->fd[fd];
    running->fd[fd] = 0;

    return 0;
}

// create a pipe, return 2 file descriptors, e.g.
// even for READ, odd for WRITE
int kpipe(int pd[2])
{
    // pd[0] = Read 
    // pd[1] = Write

    // create a pipe; fill pd[0] pd[1] (in USER mode!!!) with descriptors
    return SUCCESS;
}

int close_pipe(int fd)
{
    OFT *op; 
    PIPE *pp;

    printf("P%d close_pipe: fd=%d\n", running->pid, fd);

    op = running->fd[fd];
    running->fd[fd] = 0;                 // clear fd[fd] entry 

    if(op->mode == READ_PIPE)
    {
        pp = op->pipe_ptr;
        pp->nreader--;                   // dec n reader by 1

        if(--op->refCount == 0)
        { // last reader
            if(pp->nwriter <= 0)
            { // no more writers
                pp->busy = 0;             // free the pipe   
                return SUCCESS;
            }
        }
        // JJ: changed "wakeup" to "kwakeup" and (int)
        kwakeup((int)&pp->room);          // wakeup any WRITER on pipe 
        return SUCCESS; 
    }

    // YOUR CODE for the WRITE_PIPE case:

    return SUCCESS;
}
