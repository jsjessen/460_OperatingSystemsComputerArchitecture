#include "kernel.h"

#define interrupts_off() { save    CPU.SR;  mask out interrupts;}
#define interrupts_on()  { restore CPU.SR to saved SR; }

#define LOCK(x)      while(TS(x));
#define UNLOCK(x)    x=0;


typedef struct semaphore
{
    int value;
    PROC *queue; // FIFO
} SEMAPHORE;

//SEMAPHORE s;        s.value = INITIAL_VALUE; s.queue = NULL;

int P(SEMAPHORE *s)
{
    interrupts_off();      /* see above macro #defines */
    LOCK(s->lock);         /* keep trying on the spin lock */

    s->value--;
    if (s->value < 0)
        wait(s->queue);   /* block caller;
                             enqueue caller to s->queue);
                             UNLOCK(s->lock);
                             switch process;
                             <================= resume point !!!!  
                             else                 */   
    UNLOCK(s->lock);

    // <==== /* resume point is here if waited in s-queue */ 
        interrupt_on();          /* restore saved CPU status Reg */

}

int V(SEMAPHORE *s)
{
    interrupts_off();
    LOCK(s->lock);

    s->value++;
    if (s->value <= 0)
        signal(s->queue); /* dequeue and wakeup
                             first waiter from s->queue 
                             */
    UNLOCK(s->lock);
    interrupts_on(); 
}
