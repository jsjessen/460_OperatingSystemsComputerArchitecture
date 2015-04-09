int ksleep(int event)
{
   YOUR ksleep
}
  
int kwakeup(int event)
{
   YOUR kwakeup
}

int kwait(int *status)
{
  YOUR kwait
}




int kexit(int exitValue)
{
  YOUR kexit as before but ADD:

  // ADD: close opened file descriptors
  for (i=0; i<NFD; i++){
      if (running->fd[i] != 0)
         close_pipe(i);
  }
  tswitch();
}

