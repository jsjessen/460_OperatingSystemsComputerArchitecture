#include "io.h"
#include "type.h"


// Add process to priority queue
void enqueue(PROC** queue, PROC* new)
{
    PROC* p = *queue;

    // If empty queue, create first element
    if(!(*queue))
    {
        *queue = new;
        new->next = NULL;
        return;
    }

    // Go to insertion point
    while(p->next && (p->next->priority >= new->priority))
        p = p->next;

    // Insert
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
    while(p->next) 
    {
        // [PID, Priority]
        printf("[%d, %d] --> ", p->pid, p->priority);
        p = p->next;
    }
    printf("NULL");
}
