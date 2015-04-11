#include "kernel.h"
//#include "fcntl.h" // Open()

char *MODE[] = { "READ_PIPE ", "WRITE_PIPE" };

void show_pipe(PIPE *p)
{
    printf("------------ PIPE CONTENTS ------------\n");     
    printf("Data = %d                  Room = %d   \n", p->data, p->room);
    printf("#readers = %d             #writers = %d\n", p->nreader, p->nwriter);
    printf("Contents = \"%s\"                    \n\n", p->buf);
    printf("---------------------------------------\n");
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

    printf("=========================\n");
    printf("   FD     Mode     Ref       \n");
    printf("  ----   ------   -----      \n");
    for(i = 0; i < NFD; i++)
    {
        if((op = running->fd[i]) != NULL)
            printf("   %d      %s      %d \n", i, op->mode, op->refCount);
    }
    printf("=========================\n");

    return SUCCESS;
}

int read_pipe(int fd, char *buf, int n)
{
    int i;

    PIPE* pp = running->fd[fd]->pipe_ptr;

    printf("Before P%d read from the pipe: fd=%d\n", running->pid, fd);
    show_pipe(pp);

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
    //      ---------------------------------------------- 

    i = 0;
    while(i < n)
    {
        // Read data
        for( ; i < n && pp->data > 0; i++)
        {
            pp->tail %= PSIZE;
            buf[i] = pp->buf[pp->tail++]; // THINK ABOUT 
            pp->data--;
            pp->room++;
        }
        printf("After P%d read from the pipe: fd=%d\n", running->pid, fd);
        show_pipe(pp);

        // Wake writers that were waiting for room 
        if(i > 0) kwakeup((int)&pp->room); 

        // Wait for data to be written to the pipe
        if(pp->data <= 0) kwait(&pp->data);
    }

    printf("P%d has finished writing to the pipe: fd=%d\n", running->pid, fd);
    return i;
}

// Tries to write nbytes of data to the pipe
int write_pipe(int fd, char *buf, int n)
{
    int i;

    PIPE* pp = running->fd[fd]->pipe_ptr;

    printf("Before P%d writes to the pipe: fd=%d\n", running->pid, fd);
    show_pipe(pp);

    //           ------------------------------------------------------------------
    //      (1). If (no READER on the pipe) return BROKEN_PIPE_ERROR;
    //           ------------------------------------------------------------------
    //
    // (pipe still has READERs):
    //
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

    i = 0;
    while(i < n)
    {
        if(pp->nreader <= 0)
        {
            printf("Error write_pipe(fd=%d): No readers - BROKEN PIPE\n", fd);
            return FAILURE;
        }

        // Write data
        for( ; i < n && pp->room > 0; i++)
        {
            pp->head %= PSIZE;
            pp->buf[pp->head++] = buf[i]; // THINK ABOUT 
            pp->data++;
            pp->room--;
        }
        printf("After P%d wrote to the pipe:\n", running->pid, fd);
        show_pipe(pp);

        // Wake readers that were waiting for data
        if(i > 0) kwakeup((int)&pp->data); 

        // Wait for reader to read and make room in the pipe
        if(pp->room <= 0) kwait(&pp->room);
    }

    printf("P%d has finished writing to pipe: fd=%d\n", running->pid, fd);
    return i;
}

int get_free_oft(int start)
{
    int i;
    for(i = start; i < NOFT; i++)
        if(oft[i].refCount <= 0) return i;

    printf("No free OFTs!\n");
    return FAILURE;
}
int get_free_pipe(int start)
{
    int i;
    for(i = start; i < NPIPE; i++)
        if(!pipe[i].busy) return i;

    printf("No free pipes!\n");
    return FAILURE;
}
int get_free_fd(int start)
{
    int i;
    for(i = start; i < NFD; i++)
        if(!running->fd[i]) return i;

    printf("No free FDs!\n");
    return FAILURE;
}

// Create a pipe
int kpipe(int pd[2])
{
    int index;
    PIPE* pp;
    OFT* read_op;
    OFT* write_op;
    int read_fd;
    int write_fd;

    // Get a free pipe
    if((index = get_free_pipe(0)) < 0) return FAILURE;
    pp = &pipe[index];

    // Get 2 free OFTs
    if((index = get_free_oft(0)) < 0) return FAILURE;
    read_op = &oft[index];
    if((index = get_free_oft(++index)) < 0) return FAILURE;
    write_op = &oft[index];

    // Get 2 free FDs
    if((index = get_free_fd(0)) < 0) return FAILURE;
    read_fd = index; 
    if((index = get_free_fd(++index)) < 0) return FAILURE;
    write_fd = index; 

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

    // Fill pd[0] and pd[1] in User Mode with FDs 
    put_word(read_fd, running->uss, (u16)&pd[0]);
    put_word(write_fd, running->uss, (u16)&pd[1]);

    return SUCCESS;
}

int close_pipe(int fd)
{
    OFT* op = running->fd[fd];
    PIPE* pp = op->pipe_ptr;

    printf("P%d close_pipe: fd=%d\n", running->pid, fd);

    running->fd[fd] = 0;             // clear fd[fd] entry 

    if(op->mode == READ_PIPE)
    {
        pp->nreader--;               // decrement # of readers 

        if(--op->refCount == 0)
        { // last reader
            if(pp->nwriter <= 0)
            { // no more writers
                pp->busy = false;    // free the pipe   
                return SUCCESS;
            }
        }

        kwakeup((int)&pp->room);          // wakeup any Writer on the pipe 
        return SUCCESS; 
    }

    if(op->mode == WRITE_PIPE)
    {
        pp->nwriter--;               // decrement # of writers

        if(--op->refCount == 0)
        { // last writer 
            if(pp->nreader <= 0)
            { // no more readers 
                pp->busy = false;    // free the pipe   
                return SUCCESS;
            }
        }

        kwakeup((int)&pp->data);          // wakeup any READER on pipe 
        return SUCCESS; 
    }

    printf("Unable to close pipe connected to fd[%d] because of unknown mode\n", fd);
    return FAILURE;
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
