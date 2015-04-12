#include "kernel.h"

void show_pipe(PIPE* pp)
{
    char buf[PSIZE + 1];

    strcpy(buf, pp->buf);
    buf[PSIZE] = '\0';

    printf("------------------ PIPE ------------------\n");
    printf("    Data = %d               Room = %d     \n", pp->data, pp->room);
    printf("    #readers = %d           #writers = %d\n", pp->nreader, pp->nwriter);
    printf("    Head = %d               Tail = %d     \n", pp->head, pp->tail);
    printf("------------------------------------------\n");
    printf(" |%s|\n", buf);
    printf("==========================================\n");
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

    printf("============================= \n");
    printf("   FD       Mode       Ref    \n");
    printf("  ----   ----------   -----   \n");
    for(i = 0; i < NFD; i++)
    {
        if((op = running->fd[i]) != NULL)
            printf("   %d     %s     %d \n", i, modes[op->mode], op->refCount);
    }
    printf("============================= \n");

    return SUCCESS;
}

// Tries to read n bytes of data from the pipe
int read_pipe(int fd, char *buf, int n)
{
    int i = 0;

    OFT* op = running->fd[fd];
    PIPE* pp = op->pipe_ptr;

    if(!op || op->refCount <= 0 || op->mode != READ_PIPE)
    {
        printf("Read pipe failed: FD %d is not open for pipe reading\n", fd);
        pfd();
        return FAILURE;
    }

    // While the # bytes read is less than the # bytes requested
    while(i < n)
    {
        int snapshot = i;

        // If the pipe is empty, wait for the writer to add data
        if(pp->data <= 0)
        {
            printf("The pipe is empty => P%d waits for data (fd=%d)\n", 
                    running->pid, fd);
            ksleep((int)&pp->data);
        }
        // There must be data in the pipe now

        show_pipe(pp);
        printf("P%d tries to read %d bytes from the pipe (fd=%d)\n", 
                running->pid, n - i, fd);
        getc();

        // Put data from the pipe into the supplied buffer
        for(; i < n && pp->data > 0; i++)
        {
            pp->tail %= PSIZE;
            put_byte(pp->buf[pp->tail++], running->uss, (u16)&buf[i]);
            pp->data--;
            pp->room++;

            // Make read space with visual indication for show_pipe()
            pp->buf[pp->tail - 1] = '-'; 
        }
        // There is probably room in the pipe now

        // If there is room in the pipe, wakeup writers waiting for room 
        if(pp->room > 0)
        {
            show_pipe(pp);
            printf("P%d read %d bytes from the pipe => Wakes writers (fd=%d)\n", 
                    running->pid, i - snapshot, fd);
            kwakeup((int)&pp->room); 
        }
        else
            printf("P%d failed to read data from the pipe (fd=%d)\n", 
                    running->pid, fd);
    }
    printf("P%d completes read job: %d/%d bytes (fd=%d)\n", 
            running->pid, i, n, fd);

    return i;
}

// Tries to write n bytes of data to the pipe
int write_pipe(int fd, char *buf, int n)
{
    int i = 0;

    OFT* op = running->fd[fd];
    PIPE* pp = op->pipe_ptr;

    if(!op || op->refCount <= 0 || op->mode != WRITE_PIPE)
    {
        printf("Write pipe failed: FD %d is not open for pipe writing\n", fd);
        pfd();
        return FAILURE;
    }

    // While the # bytes written is less than the # bytes requested
    while(i < n)
    {
        int snapshot = i;

        // If there are no readers to read data from the pipe,
        // don't bother writing data to it
        if(pp->nreader <= 0)
        {
            printf("Error: Broken Pipe (fd=%d)\n", fd);
            return FAILURE;
        }
        // There must be at least one reader

        // If the pipe is full, wait for the reader to make room
        if(pp->room <= 0) 
        {
            printf("The pipe is full => P%d waits for room (fd=%d)\n", 
                    running->pid, fd);
            ksleep((int)&pp->room);
        }
        // There must be room in the pipe now

        show_pipe(pp);
        printf("P%d tries to write %d bytes to the pipe (fd=%d)\n", 
                running->pid, n - i, fd);
        getc();

        // Get data from the supplied buffer and write it to the pipe
        for(; i < n && pp->room > 0; i++)
        {
            pp->head %= PSIZE;
            pp->buf[pp->head++] = get_byte(running->uss, (u16)&buf[i]);
            pp->data++;
            pp->room--;
        }
        // There is probably data in the pipe now

        // If there is data in the pipe, wakeup readers waiting for data 
        if(pp->data > 0)
        {
            show_pipe(pp);
            printf("P%d wrote %d bytes to the pipe => Wakes readers (fd=%d)\n", 
                    running->pid, i - snapshot, fd);
            kwakeup((int)&pp->data); 
        }
        else
            printf("P%d failed to write data to the pipe (fd=%d)\n", 
                    running->pid, fd);
    }
    printf("P%d completes write job: %d/%d bytes (fd=%d)\n", 
            running->pid, i, n, fd);

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
    int i;
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
    for(i = 0; i < PSIZE; i++) pp->buf[i] = '-';

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

    pfd();

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
        pfd();
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
        pfd();
        return SUCCESS; 
    }

    printf("Unable to close pipe connected to fd[%d] because of unknown mode\n", fd);
    pfd();
    return FAILURE;
}
