#include "io.h"
#include "type.h"


// Add process to priority queue
void enqueue(PROC** queue, PROC* new)
{
    PROC* p = *queue;

    // If empty queue, create first element
    if(!p)
    {
        printf("Q FIRST ELEMENT p = %d\n", p->pid); 
        *queue = new;
        new->next = NULL;
        return;
    }

    if(p->priority < new->priority)
    {
        // Insert at head
       *queue = new;
       new->next = p;
       return;
    }

    // Go to insertion point
    while((new->priority <= p->next->priority) && p->next )
        p = p->next;

    // Insert after p
    new->next = p->next;
    p->next = new;
}

// Remove process from priority queue
PROC* dequeue(PROC** queue)
{
    PROC* p = *queue;

    if(!(*queue))
    {
        printf("Unable to dequeue because queue is empty\n");
        return NULL;
    }
    *queue = (*queue)->next;

    p->next = NULL;
    return p; 
}

// Print contents of queue
void printQueue(char* name, PROC* queue)
{
    PROC* p = queue;

    printf("%s = ", name);
    while(p) 
    {
        // [PID, Priority]
        printf("[%d, %d]->", p->pid, p->priority);
        p = p->next;
    }
    printf("0\n");
}
