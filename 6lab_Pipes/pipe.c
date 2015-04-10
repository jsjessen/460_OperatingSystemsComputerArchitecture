#include "kernel.h"

void show_pipe(PIPE *p)
{
   //int i, j;

   printf("------------ PIPE CONTENETS ------------\n");     
   // print pipe information
   printf("\n----------------------------------------\n");
}

char *MODE[ ]={"READ_PIPE ","WRITE_PIPE"};

int pfd()
{
  // print running process' opened file descriptors
    return 0;
}
//============================================================


int read_pipe(int fd, char *buf, int n)
{
  // your code for read_pipe()
    return 0;
}

int write_pipe(int fd, char *buf, int n)
{
  // your code for write_pipe()
    return 0;
}

int kpipe(int pd[2])
{
  // create a pipe; fill pd[0] pd[1] (in USER mode!!!) with descriptors
    return 0;
}

int close_pipe(int fd)
{
  OFT *op; PIPE *pp;

  printf("proc %d close_pipe: fd=%d\n", running->pid, fd);

  op = running->fd[fd];
  running->fd[fd] = 0;                 // clear fd[fd] entry 

  if (op->mode == READ_PIPE){
      pp = op->pipe_ptr;
      pp->nreader--;                   // dec n reader by 1

      if (--op->refCount == 0){        // last reader
	if (pp->nwriter <= 0){         // no more writers
	     pp->busy = 0;             // free the pipe   
             return 0; //JJ: added 0
        }
      }
      // JJ: changed "wakeup" to "kwakeup" and (int)
      kwakeup((int)&pp->room);               // wakeup any WRITER on pipe 
      return 0; //JJ: added 0
  }
  
  // YOUR CODE for the WRITE_PIPE case:

    return 0;
}
