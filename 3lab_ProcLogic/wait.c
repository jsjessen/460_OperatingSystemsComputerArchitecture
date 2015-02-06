#include "lib/type.h"
#include "lib/io.h"
#include "lib/queue.h"
#include "lib/list.h"

PROC proc[NPROC], *running, *freeList, *sleepList, *readyQueue;

int procSize = sizeof(PROC);
int nproc = 0; 
int color;

void initialize(void);
int body(void);  
PROC* kfork(void);

// running PROC to sleep on an event value
sleep()
{
}

// wakeup ALL PROCs sleeping on event
wakeup()
{
}

// to wait for a ZOMBIE child
wait()
{
}

