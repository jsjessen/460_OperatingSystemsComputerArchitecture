#include "list.h"

// Put process at end of free list
void enlist(PROC** list, PROC* new)
{
    PROC* p = *list;

    // If empty list, create frist element
    if(!(*list))
    {
        *list = new;
        new->next = NULL;
        return;
    }

    // Go to end of list
    while(p->next) 
        p = p->next;

    p->next = new;
}

// Get first process from list
PROC* delist(PROC** list)
{
    PROC* p = *list;

    if(!(*list))
    {
        printf("Unable to delist because list is empty\n");
        return NULL;
    }
    *list = (*list)->next;

    p->next = NULL;
    return p; 
}

// Print contents of list 
void printList(char* name, PROC* list)
{
    PROC* p = list;

    printf("%s = ", name);
    while(p) 
    {
        // [PID, Priority]
        printf("[%d, %d]->", p->pid, p->priority);
        p = p->next;
    }
    printf("0\n");
}
