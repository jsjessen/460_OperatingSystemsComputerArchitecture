#include "kernel.h"
//#include "fcntl.h" // Open()

char *MODE[] = { "READ_PIPE ", "WRITE_PIPE" };

void show_pipe(PIPE *p)
{
    // displays the contents of the pipe, e.g.
    //       ------------------------------------------------------   
    //        #data = number of chars currently in the pipe;
    //        #room = number of rooms available in the pipe;
    //        contents = "........."; (actual chars in the pipe).
    //       ------------------------------------------------------
    //int i, j;

    printf("------------ PIPE CONTENTS ------------\n");     
    printf("Data = %d                  Room = %d\n");
    printf("#readers = %d              #writers = %d\n");
    printf("Contents = \"%s\"\n");
    printf("\n---------------------------------------\n");
}

int pfd()
{
    int i;
    OFT *op; 

    if(!running->fd[0] && !running->fd[1])
    {
        printf("No opened files\n");
        return SUCCESS;
    }

    printf("=============================\n");
    printf("   FD    Type    Mode    Ref \n");
    printf("  ----  ------  ------  -----\n");
    for(i = 0; i < NFD; i++)
    {
        if((op = running->fd[i]) != NULL)
            printf("   %d    PIPE    %s      %d \n", i, op->mode, op->refCount);
    }
    printf("=============================\n");

    return SUCCESS;
}

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
    OFT* op = running->fd[fd];

    printf("P%d write_pipe: fd=%d\n", running->pid, fd);
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


    return 0;
}

OFT* get_free_oft()
{
    int i;
    for(i = 0; i < NOFT; i++)
        if(oft[i].refCount <= 0) return &oft[i];

    printf("No free OFTs!\n");
    return NULL;
}

PIPE* get_free_pipe()
{
    int i;
    for(i = 0; i < NPIPE; i++)
        if(!pipe[i].busy) return &pipe[i];

    printf("No free pipes!\n");
    return NULL;
}

int get_free_fd()
{
    int i;
    for(i = 0; i < NFD; i++)
        if(!running->fd[i]) return i;

    printf("No free FDs!\n");
    return FAILURE;
}

// create a pipe, return 2 file descriptors, e.g.
// even for READ, odd for WRITE
int kpipe(int pd[2])
{
    PIPE* pp;
    OFT* read_op;
    OFT* write_op;
    int read_fd;
    int write_fd;

    // Get a free pipe, 2 free OFTs, and 2 free FDs
    if(!(pp = get_free_pipe())) return FAILURE;
    if(!(read_op = get_free_oft())) return FAILURE;
    if(!(write_op = get_free_oft())) return FAILURE;
    if((read_fd = get_free_fd()) < 0) return FAILURE;
    if((write_fd = get_free_fd()) < 0) return FAILURE;

    // Initialize Pipe
    pp->room = PSIZE;
    pp->data = 0;
    pp->head = 0;
    pp->tail = 0;
    pp->nreader = 1;
    pp->nwriter = 1;
    pp->busy = true;

    // Initialize Reader
    read_op->mode = READ_PIPE;
    read_op->refCount = 1;
    read_op->pipe_ptr = pp;
    running->fd[read_fd] = read_op;

    // Initialize Writer 
    write_op->mode = WRITE_PIPE; 
    write_op->refCount = 1;
    write_op->pipe_ptr = pp;
    running->fd[write_fd] = write_op;

    // Fill pd[0] pd[1] in User Mode with FDs 
    put_word(read_fd, running->uss, (u16)&pd[0]);
    put_word(write_fd, running->uss, (u16)&pd[1]);

    return SUCCESS;
}

int close_pipe(int fd)
{
    OFT *op; 
    PIPE *pp;

    printf("P%d close_pipe: fd=%d\n", running->pid, fd);

    op = running->fd[fd];
    pp = op->pipe_ptr;
    running->fd[fd] = 0;             // clear fd[fd] entry 

    if(op->mode == READ_PIPE)
    {
        pp->nreader--;               // dec n reader by 1

        if(--op->refCount == 0)
        { // last reader
            if(pp->nwriter <= 0)
            { // no more writers
                pp->busy = false;    // free the pipe   
                return SUCCESS;
            }
        }

        kwakeup((int)&pp->room);          // wakeup any WRITER on pipe 
        return SUCCESS; 
    }

    if(op->mode == WRITE_PIPE)
    {
        pp->nwriter--;               // dec n reader by 1

        if(--op->refCount == 0)
        { // last writer 
            if(pp->nreader <= 0)
            { // no more readers 
                pp->busy = false;    // free the pipe   
                return SUCCESS;
            }
        }

        kwakeup((int)&pp->room);          // wakeup any READER on pipe 
        return SUCCESS; 
    }

    return SUCCESS;
}

// -------------------------------------------------------------------------------
// 
// (2). P1 does pid = fork(); creates P2 and COPY P1' fd's ==> inc OFT's refCount
//      to 2 and nreaders, nwriters to 2 also: 
// 
// 
// PROC[1]                                              PROC[2]
// ======                                              ========
// 
// 
// fd[0] --->  OFT      <------------------------------ fd[0]
//           ==========
//           R_PIPE
//           refCount=1 (2)
//            Ptr -------->      PIPE
//           ==========   -> ===============
//                        |  char buf[PSIZE]
//                        |    head=tail=0
//                        |    nreader = 1 (2)
//                        |    nwriter = 1 (2)
//                        |  ===============
//                        |
// fd[1] -->    OFT   <------------------------------  fd[1]
//            ========    |
//             W_PIPE     |
//          refCount=1(2) |
//              Ptr -------
// ======     ========                                 =======   
// 
// -------------------------------------------------------------------------------
// 
// (3). Let parent P1 be WRITER        |        child P2 be READER
//      P1: close(pd[0]);              |        P2: close(pd[1]);
// 
// 
// 
// PROC[1]                                              PROC[2]
// ======                                              ========
// 
// 
// fd[0]=0      OFT      <------------------------------ fd[0]
//           ==========
//           R_PIPE
//           refCount=1 (1)
//            Ptr -------->      PIPE
//           ==========   -> ===============
//                        |  char buf[PSIZE]
//                        |    head=tail=0
//                        |    nreader = 1 (1)
//                        |    nwriter = 1 (1)
//                        |  ===============
//                        |
// fd[1] -->    OFT                                     fd[1]=0
//            ========    |
//             W_PIPE     |
//          refCount=1(1) |
//              Ptr -------
// ======     ========                                 =======   
// 
// -------------------------------------------------------------------------------
// 
// (3). P1 can only WRITE to pipe by pd[1];   P2 can only READ from pipie by pd[0]
// 
//                      P1                    P2
//                 WRITE pd[1] --> PIPE --> READ pd[0]
// 
// 
//      OR          P1 -->WRITE--> PIPE --> READ --> P2
// 
//      OR                       P1  |  P2
// 
// -------------------------------------------------------------------------------
// (4). P1 writes to PIPE;           |           P2 reads from PIPE
//                                   |
//   n = write(pd[1], wbuf, nbytes); |       n = read(pd[0], rbuf, nbytes);    
// -------------------------------------------------------------------------------
// 
// (5). write()/read()  fd of PIPE are governed by 4(a) and 4(b).
// 
